#include "RPS.hpp"

#include <iostream>
#include <random>
#include <sstream>
#include <exception>
#include <ctime>

#include "Message.hpp"
#include "TCPConnection.hpp"

#include "log.hpp"


void mergeAmongReplaceable(
    std::shared_ptr<View> vInto, std::shared_ptr<View> vFrom,
    std::shared_ptr<View> vRepl, unsigned int maxSize);

RPS::RPS(const std::shared_ptr<Descriptor> myself, const std::vector<std::string>& bsIPs,
         unsigned int viewSize, unsigned int gossipSize, 
         unsigned int period, bool debug) 
    : mMyself(myself), mView(std::make_shared<View>()),
    mListener(std::make_shared<TCPListener>(RPS_TCP_PORT)), 
    mSendReqCnt(0),mRecReqCnt(0), 
    mViewSize(viewSize), mGossipSize(gossipSize), 
    mPeriod(period), mTimeout (RPS_CONN_TIMEOUT),
    mBootstrapIPs(bsIPs), mDebug(debug) {

  std::stringstream ss;
  ss << "RPS initialized:" << std::endl;
  ss << "\tPeriod = " << mPeriod.count() << "ms" << std::endl;
  ss << "\tGossipSize = " << mGossipSize << std::endl;
  ss << "\tViewSize = " << mViewSize << std::endl;
  LOG(ss.str().c_str());

  mListenerThread = std::thread(&RPS::listenerDaemon, this);
  mSenderThread = std::thread(&RPS::senderDaemon, this);
}

// RPS::~RPS() {
//   // Thread destructor:
//   // "If the thread is joinable when destroyed, terminate() is called."
//   // => No need for join, which would hang
//   // mListenerThread.join();
//   // mSenderThread.join();
// }


void RPS::listenerDaemon() {
  DEBUG(mDebug, "Listener daemon started.");
  
  while (true) {
    if (mListener->IsClosed()) {
      DEBUG(mDebug, "Listener is closed: terminating thread.");
      break;
    }

    std::shared_ptr<TCPConnection> conn;
    try {
      conn = mListener->Accept();
    } catch (std::exception &e) {
      std::stringstream ss;
      ss << "Error accepting connection: " << e.what(); 
      LOG(ss.str().c_str());
      continue;
    }

    // https://news.ycombinator.com/item?id=13332567
    //  You should never, ever, create one thread per socket. 
    // The choice is between select, polled IO, or events. 
    // ... Fuck you man!
    // Not tested much
    //std::thread(&RPS::receiveRequest, this, conn);

    // We receive on the same thread to reduce context switching costs
    // At the expense of latency/possibly missing requests
    receiveRequest(conn);
  }
}

void RPS::senderDaemon() {
  DEBUG(mDebug, "Sender daemon started.");
  using namespace std::chrono;

  steady_clock::time_point tStart;
  while (true) {
    tStart = steady_clock::now();

    sendRequest();

    auto dt = (steady_clock::now() - tStart);
    if (dt < mPeriod) {
      std::this_thread::sleep_for(mPeriod - dt);
    }
  }
}

void RPS::sendRequest() {
  std::stringstream ss;

  mMutex.lock();
  auto cnt = mSendReqCnt;
  mSendReqCnt++;

  // Return if we don't know any peer
  if (mView->Size() == 0 && mBootstrapIPs.size() == 0) {
    LOG("I don't know any peer.");
    mMutex.unlock();
    return;
  }

  // 1 - Increase by one the age of all neighbors
  mView->IncrementAge();

  // Select the recipient Q
  std::shared_ptr<Descriptor> q;
  std::string qIP;
  if (mView->Size() == 0) {
    // Select Q from bootstrap list
    int idx = std::default_random_engine()() % mBootstrapIPs.size();
    qIP = mBootstrapIPs[idx];
  } else {
    // 2 - Select neighbor Q with the highest age among all neighbors [...]
    q = mView->Oldest();
    qIP = q->IP();
    // Remove Q from our view
    mView->Remove(q->ID());
  }

  // 2 - Select [...] l-1 other random neighbors
  auto vSnd = mView->RandomSubset(mGossipSize - 1);

  // 3 - Replace Q's entry w/ a new entry of age 0 and with P's address
  // That is: add my descriptor (has age of 0) to the view we will send
  mMyself->UpdateTimestamp();
  vSnd->Add(*mMyself);

  // Release the lock
  mMutex.unlock();

  // 4 - Send the updated subset to Q
  Message sndMess = Message{mMyself, vSnd};
  std::shared_ptr<TCPConnection> conn;
  try {
    // Dial q
    conn = TCPConnection::Dial(qIP, RPS_TCP_PORT);
  } catch (std::exception& e) {
    ss << "Error dialing " << qIP << ": " << e.what() << std::endl;
    LOG(ss.str().c_str());
    return;
  }
  std::iostream ios(conn.get());
  try {
    // Send it the Message
    sndMess.Marshal(ios);
  } catch (std::exception& e) {
    ss << "Error sending RPS_REQ #" << cnt << " to " << qIP << ": " << e.what();
    LOG(ss.str().c_str());
    return;
  }

  ss << "Sent RPS_REQ #" << cnt << " to " << qIP << ": " << vSnd->String();
  DEBUG(mDebug, ss.str().c_str());
  ss.clear();
  ss.str("");

  // 5 - Receive from Q ... 
  std::shared_ptr<Message> rcvdMess;
  try {
    rcvdMess = Message::FromStream(ios);
  } catch (std::exception &e) {
    ss << " Error receiving RPS_RESP #" << cnt << " from " << qIP << ": " << e.what();
    LOG(ss.str().c_str());
    conn->Close();
    return;
  }

  // ... a subset of no more that l of its own entries.
  if (rcvdMess->V->Size() > mGossipSize)
    rcvdMess->V = rcvdMess->V->RandomSubset(mGossipSize);

  ss << "Received RPS_RESP #" << cnt << " from " << qIP << 
    ": " << rcvdMess->V->String();
  DEBUG(mDebug, ss.str().c_str());
  // ss.clear();
  // ss.str("");

  mergeView(rcvdMess->V, vSnd);

  conn->Close();
}

void RPS::receiveRequest(std::shared_ptr<TCPConnection> conn) {
  std::stringstream ss;
  auto cnt = mRecReqCnt++;


  // Unpack Request from connection, answer, handle Request
  std::iostream ios(conn.get());
  std::shared_ptr<Message> rcvdMess;
  try {
    rcvdMess = Message::FromStream(ios);
  } catch (std::exception &e) {
    ss << " Error receiving RPS_REQ #" << cnt << " from " << conn->RemoteIP() << ": " << e.what();
    LOG(ss.str().c_str());
    // Only active loop closes conn?
    conn->Close();
    return;
  }

  ss << "Received RPS_REQ #" << cnt << " from " << conn->RemoteIP() << 
    ": " << rcvdMess->V->String();
  DEBUG(mDebug, ss.str().c_str());
  ss.clear();
  ss.str("");

  mMutex.lock(); // Lock the mutex for view R/W

  // 2 - Select [...] l-1 other random neighbors
  auto vSnd = mView->RandomSubset(mGossipSize - 1);

  // 3 - Replace Q's entry w/ a new entry of age 0 and with P's address
  // That is: add my descriptor (has age of 0) to the view we will send
  mMyself->UpdateTimestamp();
  vSnd->Add(*mMyself);

  mMutex.unlock();

  Message sndMess = Message{mMyself, vSnd};
  try {
    // Send it the Message
    sndMess.Marshal(ios);
  } catch (std::exception& e) {
    ss << "Error sending RPS_RESP #" << cnt << " to " << conn->RemoteIP() << ": " << e.what();
    LOG(ss.str().c_str());
    return;
  }

  ss << "Sent RPS_RESP  #" << cnt << " to " << conn->RemoteIP() << ": " << vSnd->String();
  DEBUG(mDebug, ss.str().c_str());
  ss.clear();
  ss.str("");

  mergeView(rcvdMess->V, vSnd);

  // Only active loop closes conn?
  conn->Close();
}

void RPS::mergeView(std::shared_ptr<View> vRcvd, std::shared_ptr<View> vRepl) {
  // 6 - Discard entries pointing at P ...
  vRcvd->Remove(mMyself->ID());
  // ... and entries already contained in P's cache
  mMutex.lock(); // Lock the mutex for view R/W
  vRcvd->Diff(*mView);

  // 7 - Update P's cache to include all remaining entries,
  // by firstly using empty cache slots (if any),
  // and secondly replacing entries among the ones sent to Q
  mergeAmongReplaceable(mView, vRcvd, vRepl, mViewSize);

  std::stringstream ss;
  //ss << "Updated our view, which now contains " << mView->Size() << " elements.";
  ss << "Updated our view: " << mView->String() << std::endl;
  DEBUG(mDebug, ss.str().c_str());

  mMutex.unlock();
}

void mergeAmongReplaceable(
    std::shared_ptr<View> vInto, std::shared_ptr<View> vFrom,
    std::shared_ptr<View> vRepl, unsigned int maxSize) {
  // First make room into the output view by removing the replaceable elements
  vInto->Diff(*vRepl);

  // Then add items from the input view v2
  for (auto &it: vFrom->Map()) {
    if (vInto->Size() >= maxSize) 
      break;

    vInto->Add(*it.second);
  }

  // Finally, fill remaining place in v with items from repl
  for (auto &it: vRepl->Map()) {
    if (vInto->Size() >= maxSize) 
      break;

    vInto->Add(*it.second);
  }
}


