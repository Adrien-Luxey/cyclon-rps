#include "net_utils.hpp"

#include <arpa/inet.h> // htons, ltohs, htonl, ntohl


std::string sockaddrToIP(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    struct in_addr ipAddr = ((struct sockaddr_in*)sa)->sin_addr;
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &ipAddr, ipStr, INET_ADDRSTRLEN );
    return std::string(ipStr);
  } else if (sa->sa_family == AF_INET6) {
    struct in6_addr ip6Addr = ((struct sockaddr_in6*)sa)->sin6_addr;
    char ipStr[INET6_ADDRSTRLEN];
    inet_ntop( AF_INET6, &ip6Addr, ipStr, INET6_ADDRSTRLEN );
    return std::string(ipStr);
  } else {
    return std::string();
  }
}