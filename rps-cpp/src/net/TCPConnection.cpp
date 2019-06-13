#include "TCPConnection.hpp"

#include <arpa/inet.h> // htons, ltohs, htonl, ntohl
#include <cstring> // memset
#include <stdexcept>
#include <sys/socket.h> // listen, bind, etc
#include <sys/types.h> // listen, bind, etc
#include <unistd.h> // write
#include <iostream>

#include "error.hpp"


std::shared_ptr<TCPConnection> TCPConnection::Dial(std::string remoteIP, 
  int remotePort, duration timeout) {
  auto conn = std::make_shared<TCPConnection>(
    socket(PF_INET, SOCK_STREAM, 0), remoteIP, timeout);

  struct sockaddr_in remoteAddr;
  memset((char*)&remoteAddr, 0, sizeof(remoteAddr));
  remoteAddr.sin_family       = AF_INET;
  remoteAddr.sin_port         = htons(remotePort);
  inet_pton(AF_INET, remoteIP.c_str(), &(remoteAddr.sin_addr));

  while (connect(conn->SocketID(), (struct sockaddr*)&remoteAddr, 
              sizeof(remoteAddr)) != 0) {
    switch(errno) {
    case EAGAIN: 
      {
        continue;
      }
    default:
      {
        conn->Close();
          throw std::runtime_error(
            buildErrorMessage("TCPConnection::", __func__, ": connect: ", 
                              strerror(errno)));
      }
    }
  }

  return conn;
}

// - streambuf virtual methods
// On error, throw exception (or always return bufSize) or return -1?
std::streamsize TCPConnection::xsputn(
  const char* buf, std::streamsize bufSize) {
  std::streamsize totalSent = 0;

  while (totalSent < bufSize) {
    // MSG_NOSIGNAL: Do not emit MSG_NOSIGNAL when EPIPE (closed pipe)
    std::streamsize nSent = send(SocketID(), 
                                  buf + totalSent, 
                                  bufSize - totalSent,
                                  MSG_NOSIGNAL);
    if (nSent == 0) {
      break;
    } else if (nSent == static_cast<std::streamsize>(-1)) {
      switch(errno) {
        // case EBADF: // An invalid descriptor was specified. 
        // case ECONNRESET: // Connection reset by peer. 
        // case EINTR: // A signal occurred before any data was transmitted.
        // case EINVAL: // Invalid argument passed. 
        // case ENOMEM: // No memory available.
        // case ENOTCONN: // The socket is not connected, and no target has been given. 
        // case ENOTSOCK: // The argument sockfd is not a socket.
        // case ENXIO: // No such device or address (not send error).
        // case EPIPE: // The local end has been shut down on a connection oriented socket.
        // {
        //     // Fatal error or programming bug
        //     throw std::domain_error(
        //       buildErrorMessage("TCPConnection::", __func__, 
        //                         ": send: critical error: ", strerror(errno)));
        // }
        // case EDQUOT:
        // case EFBIG:
        // case EIO:
        // case ENETDOWN:
        // case ENETUNREACH:
        // case ENOSPC:
        // {
        //     // Resource acquisition failure or device error
        //     throw std::runtime_error(
        //       buildErrorMessage("TCPConnection::", __func__, 
        //                         ": send: resource failure: ", 
        //                         strerror(errno)));
        // }
        // The socket is marked nonblocking and the requested operation would block.
        case EAGAIN: 
        {
          // Retry the read
          continue;
        }
        default:
        {
          throw std::runtime_error(
            buildErrorMessage("TCPConnection::", __func__, 
                              ": send: returned -1: ", strerror(errno)));
        }
      }
    }

    totalSent += nSent;
  }

  return totalSent;
}

std::streamsize TCPConnection::xsgetn(
  char* buf, std::streamsize bufSize) {
  std::streamsize totalRcvd = 0;

  while (totalRcvd < bufSize) {
    // MSG_NOSIGNAL: Do not emit MSG_NOSIGNAL when EPIPE (closed pipe)
    std::streamsize nRcvd = recv(SocketID(), 
                                  buf + totalRcvd, 
                                  bufSize - totalRcvd,
                                  MSG_NOSIGNAL);

    if (nRcvd == 0) {
      break;
    } else if (nRcvd == static_cast<std::streamsize>(-1)) {
      switch(errno) {
        // The socket is marked nonblocking and the requested operation would block.
        case EAGAIN: 
        {
            // Retry the read
            continue;
        }
        default:
        {
            throw std::runtime_error(
              buildErrorMessage("TCPConnection::", __func__, 
                                ": send: returned -1: ", strerror(errno)));
        }
      }
    }
    totalRcvd += nRcvd;
  }

  return totalRcvd;
}