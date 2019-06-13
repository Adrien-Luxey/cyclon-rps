#ifndef NET_TCPCONNECTION_H_
#define NET_TCPCONNECTION_H_

#include <streambuf>
#include <string>
#include <memory>
#include <chrono>

#include "BaseSocket.hpp"

// Docs: http://www.angelikalanger.com/IOStreams/Excerpt/excerpt.htm

#define BUFFER_SIZE = 10240

typedef std::chrono::duration<int,std::milli> duration;

// Who supports IPv6 anyway?
class TCPConnection: public BaseSocket, public std::streambuf {
 public:
  TCPConnection(int socketID, std::string remoteIP, 
    duration timeout=duration::zero()) 
      : BaseSocket(socketID), mRemoteIP(remoteIP), 
      mTimeout(timeout) {}
  // https://isocpp.org/wiki/faq/ctors#named-ctor-idiom
  static std::shared_ptr<TCPConnection> 
    Dial(const std::string remoteIP, const int remotePort,
      duration timeout=duration::zero());

  const std::string RemoteIP() const { return mRemoteIP; }

  // int Read(bytes...) 
  // int Write(bytes...)

 private:
  // - streambuf virtual methods
  std::streamsize xsputn(const char* buf, std::streamsize bufSize) final;
  std::streamsize xsgetn(char* buf, std::streamsize bufSize) final;

  std::string mRemoteIP;
  const std::chrono::milliseconds mTimeout;
};

#endif // NET_TCPCONNECTION_H_