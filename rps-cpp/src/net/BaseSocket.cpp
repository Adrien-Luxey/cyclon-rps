#include "BaseSocket.hpp" 

#include <arpa/inet.h>
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
//#include <iostream>
//#include <sstream>

#include "error.hpp"

BaseSocket::BaseSocket(int socketID)
    : mSocketID(socketID) {
  if (mSocketID == invalidSocketID)
    throw std::runtime_error(buildErrorMessage("BaseSocket::", __func__, 
                             ": bad socket: ", strerror(errno)));

  if (SOCKET_REUSE_ADDR) {
    // Allow reusing a local address unless it is ACTIVE
    // Must be called before bind
    int reuse = SOCKET_REUSE_ADDR;
    if (setsockopt(mSocketID, SOL_SOCKET, SO_REUSEADDR, 
                   (const char*)&reuse, sizeof(reuse)) < 0)
        throw std::runtime_error(
          buildErrorMessage("BaseSocket::", __func__, 
                            ": failed setsockopt SO_REUSEADDR: ", 
                            strerror(errno)));
  }
}

BaseSocket::~BaseSocket() {
  if (mSocketID == invalidSocketID) {
    // This object has been closed or moved.
    // So we don't need to call close.
    return;
  }

  //try {
    Close();
  // }
  // catch(...) {
  //   // We should log this 
  //   // TODO: LOGGING CODE HERE

  //   // If the user really want to catch close errors
  //   // they should call close() manually and handle
  //   // any generated exceptions. By using the
  //   // destructor they are indicating that failures is
  //   // an OK condition.
  // }
}

void BaseSocket::Close()
{
  if (mSocketID == invalidSocketID) {
    throw std::logic_error(buildErrorMessage("BaseSocket::", __func__, 
      ": invalid socket ID (-1)"));
  }
  while(true) {
    int state = close(mSocketID);
    if (state == 0) {
      break;
    } else if (state == invalidSocketID) {
      switch(errno) {
        case EBADF: throw std::domain_error(buildErrorMessage("BaseSocket::", __func__, ": close: EBADF: ", mSocketID, " ", strerror(errno)));
        case EIO:   throw std::runtime_error(buildErrorMessage("BaseSocket::", __func__, ": close: EIO:  ", mSocketID, " ", strerror(errno)));
        case EINTR:
        {
              // TODO: Check for user interrupt flags.
              //     Beyond the scope of this project
              //     so continue normal operations.
          break;
        }
        default:  
        {
          throw std::runtime_error(buildErrorMessage("BaseSocket::", __func__, ": close: ???:  ", mSocketID, " ", strerror(errno)));
        }
      }
    }
  }
  mSocketID = invalidSocketID;
}

void BaseSocket::swap(BaseSocket& other) noexcept {
  using std::swap;
  swap(mSocketID,   other.mSocketID);
}
BaseSocket::BaseSocket(BaseSocket&& move) noexcept
    : mSocketID(invalidSocketID) {
  move.swap(*this);
}
BaseSocket& BaseSocket::operator=(BaseSocket&& move) noexcept {
  move.swap(*this);
  return *this;
}