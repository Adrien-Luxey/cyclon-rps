#include "TCPListener.hpp"


#include <arpa/inet.h> // htons, ltohs, htonl, ntohl
#include <cstring> // memset
#include <stdexcept>
#include <sys/socket.h> // listen, bind, etc
#include <sys/types.h> // listen, bind, etc
// #include <sstream>
// #include <unistd.h> // close 

#include "error.hpp"
#include "net_utils.hpp"

TCPListener::TCPListener(int localPort) 
    : BaseSocket(socket(PF_INET, SOCK_STREAM, 0)) {

  struct sockaddr_in serverAddr;
  memset((char*)&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family       = AF_INET;
  serverAddr.sin_port         = htons(localPort);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  if (bind(SocketID(), (struct sockaddr *) &serverAddr, 
           sizeof(serverAddr)) != 0) {

    Close();
    throw std::runtime_error(
      buildErrorMessage("TCPListener::", __func__, ": bind: ", 
                        strerror(errno)));
  }

  if (listen(SocketID(), maxConnectionBacklog) != 0) {
    Close();
    throw std::runtime_error(
      buildErrorMessage("TCPListener::", __func__, ": listen: ", 
                        strerror(errno)));
  }
}

std::shared_ptr<TCPConnection> TCPListener::Accept() {
  if (SocketID() == invalidSocketID) {
    throw std::logic_error(
      buildErrorMessage("TCPListener::", __func__, ": bad socket"));
      //buildErrorMessage("TCPListener::", __func__, ": accept called on a bad socket object (this object was moved)"));
  }

  // TODO: Timeout?
  struct sockaddr_storage remoteAddr;
  socklen_t size = sizeof remoteAddr;
  int newSocket = accept(SocketID(), (struct sockaddr*)&remoteAddr, &size);
  if (newSocket == -1)
  {
    throw std::runtime_error(
      buildErrorMessage("TCPListener:", __func__, ": accept: ", 
                        strerror(errno)));
  }
  return std::make_shared<TCPConnection>(
    newSocket, sockaddrToIP((struct sockaddr*)&remoteAddr));
}