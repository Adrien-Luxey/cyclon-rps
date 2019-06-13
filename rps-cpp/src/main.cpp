#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <csignal>
#include <memory>

#include <cxxopts.hpp>

#include "rps/Descriptor.hpp"
#include "rps/RPS.hpp"

void splitString(const std::string& str, const std::string& delim,
                 std::vector<std::string> &ret);

int main(int argc, char* argv[]) {

  // register signals to stop
  auto dieAlready =  [](int signum) { exit(signum); };
  signal(SIGINT, dieAlready);
  signal(SIGTERM, dieAlready);
  signal(SIGABRT, dieAlready);

  cxxopts::Options options("rps-cpp", "The best (the only) implementation of Cyclon in C++!");

  int debug = 1;
  int rpsPeriod = 5000, rpsViewSize = 6, rpsGossipSize = 4;
  std::string ip, name, bsIpsStr;


  options.add_options()
    ("help", "Print help", cxxopts::value<int>())
    ("d,debug", "Enable debugging", cxxopts::value<int>(debug))
    ("p,period", "The RPS period (ms)", cxxopts::value<int>(rpsPeriod))
    ("vs", "The RPS view size", cxxopts::value<int>(rpsViewSize))
    ("gs", "The RPS gossip size", cxxopts::value<int>(rpsGossipSize))
    ("ip", "This node's IP", cxxopts::value<std::string>(ip))
    ("name", "This node's ID", cxxopts::value<std::string>(name))
    ("bs_ips", "Other nodes' IPs for bootstrap (comma separated)", 
      cxxopts::value<std::string>(bsIpsStr))
  ;
  auto opts = options.parse(argc, argv);

  if (opts.count("help")) {
    std::cout << options.help() << std::endl;
    return 0;
  }

  std::vector<std::string> bsIPs;
  splitString(bsIpsStr, std::string(","), bsIPs);
  auto myself = std::make_shared<Descriptor>(ip, name);
  std::cout << "I am: " << myself->String() << std::endl;

  std::cout << "Launching RPS..." << std::endl;
  auto rps = std::make_shared<RPS>(myself, bsIPs, rpsViewSize, rpsGossipSize, rpsPeriod, debug);
  //RPS(myself, bsIPs, rpsViewSize, rpsGossipSize, rpsPeriod, debug);

  while (true)
    std::this_thread::sleep_for(std::chrono::seconds(1));


  return EXIT_SUCCESS;
}

void splitString(const std::string& str, const std::string& delim,
                 std::vector<std::string> &ret) {

  if (str.size() == 0)
    return;

  auto start = 0U;
  auto end = str.find(delim);
  while (end != std::string::npos)
  {
      ret.push_back(str.substr(start, end - start));
      start = end + delim.length();
      end = str.find(delim, start);
  }
  if (start != end) 
    ret.push_back(str.substr(start, end - start));
}