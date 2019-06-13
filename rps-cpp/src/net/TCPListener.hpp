#ifndef NET_TCPLISTENER_H_
#define NET_TCPLISTENER_H_

#include <memory>

#include "BaseSocket.hpp"
#include "TCPConnection.hpp"

// RAII: Acquisition is Initiatization
class TCPListener: public BaseSocket {
 public:
  TCPListener(int localPort);

  std::shared_ptr<TCPConnection> Accept();

 private:
  static constexpr int maxConnectionBacklog = 10;

};

#endif // NET_TCPLISTENER_H_