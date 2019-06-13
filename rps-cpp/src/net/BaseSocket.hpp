#ifndef NET_SOCKET_HPP_
#define NET_SOCKET_HPP_

// Docs: 
// http://beej.us/guide/bgnet/
// https://stackoverflow.com/questions/44310329/is-it-possible-to-override-stdistream-read-method

// Inspired by: https://github.com/Loki-Astari/Examples/tree/master/Version2

#define SOCKET_REUSE_ADDR 1

// An RAII base class for handling sockets.
// Socket is movable but not copyable.
class BaseSocket {
 public:
  virtual ~BaseSocket();

  // Moveable but not Copyable
  BaseSocket(BaseSocket&& move)               noexcept;
  BaseSocket& operator=(BaseSocket&& move)    noexcept;
  void swap(BaseSocket& other)                noexcept;
  BaseSocket(BaseSocket const&)               = delete;
  BaseSocket& operator=(BaseSocket const&)    = delete;

  // User can manually Close any socket
  void Close();

  // Accessor
  bool IsClosed() const  { return mSocketID == invalidSocketID; }

 protected:
  static constexpr int invalidSocketID      = -1;

  // Designed to be a base class not used used directly.
  BaseSocket(int socketID);
  int SocketID() const { return mSocketID; }

 private:
  int     mSocketID;
};

#endif // NET_SOCKET_HPP_