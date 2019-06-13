#ifndef NET_NET_UTILS_HPP
#define NET_NET_UTILS_HPP

#include <string>
//#include <sys/socket.h> // listen, bind, etc
//#include <sys/types.h> // listen, bind, etc

std::string sockaddrToIP(struct sockaddr *sa);


#endif // NET_NET_UTILS_HPP