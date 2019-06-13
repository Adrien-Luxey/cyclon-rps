#ifndef RPS_RPS_H_
#define RPS_RPS_H_

#include <chrono>
#include <memory>
#include <vector>
#include <mutex>
#include <thread>

#include "Descriptor.hpp"
#include "View.hpp"
#include "TCPListener.hpp"

#define RPS_TCP_PORT 10337
#define RPS_CONN_TIMEOUT 100 // ms

class RPS {
 public:
  RPS(const std::shared_ptr<Descriptor> myself, 
      const std::vector<std::string>& bsIPs,
      unsigned int viewSize, unsigned int gossipSize, 
      unsigned int period, bool debug);
  //~RPS();

 private:
  void listenerDaemon();
  void senderDaemon();

  void sendRequest();
  void receiveRequest(std::shared_ptr<TCPConnection> conn);

  void mergeView(std::shared_ptr<View> vRcvd, std::shared_ptr<View> vRepl);

  // - Members 
  std::shared_ptr<Descriptor> mMyself;
  std::shared_ptr<View> mView;
  std::shared_ptr<TCPListener> mListener;
  unsigned int mSendReqCnt, mRecReqCnt;

  // - Parameters
  const unsigned int mViewSize, mGossipSize;
  const std::chrono::milliseconds mPeriod, mTimeout;
  const std::vector<std::string> mBootstrapIPs;
  const bool mDebug;

  // - Synchronization
  std::mutex mMutex;
  std::thread mListenerThread, mSenderThread;
};


#endif // RPS_RPS_H_