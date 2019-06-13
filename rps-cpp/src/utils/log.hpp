#ifndef UTILS_LOG_H_
#define UTILS_LOG_H_

#include <iostream>
#include <chrono>
#include <ctime>
#include <mutex>

std::mutex logMutex;
// Docs:
// https://www.cprogramming.com/reference/preprocessor/__LINE__.html
// https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html

void logMessage (bool log, const char* file, int line, const char* message) {
  if (log) {
    using namespace std::chrono;

    logMutex.lock();
    steady_clock::time_point p = steady_clock::now();
    milliseconds ms = duration_cast<milliseconds>(p.time_since_epoch());
    time_t t =  duration_cast<seconds>(ms).count();
    char tStr[25];
    strftime(tStr, 25, "%Y-%m-%d %H:%M:%S.", localtime(&t));

    std::cout << "[" << tStr << (ms.count() % 1000) << " " << 
      file << ":" << line << "] " << message << std::endl;
    logMutex.unlock();
  }
}


#define DEBUG( debug, message ) logMessage( (debug), __FILE__, __LINE__, (message) )
#define LOG( message ) logMessage( true, __FILE__, __LINE__, (message) )


#endif // UTILS_LOG_H_