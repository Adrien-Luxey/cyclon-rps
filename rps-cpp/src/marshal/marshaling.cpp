#include "marshaling.hpp"

#include <stdexcept>
#include <cstring>
#include <iostream>
#include <iomanip>

using namespace std;


void MarshalBool(std::ostream &out, const bool val) {
  char buf = val;
  out.write(&buf, 1);
}
void UnmarshalBool(bool* val, std::istream &in) {
  char buf;
  in.read(&buf, 1);
  *val = buf;
}

// TODO: memcpy is BAD, we should care about endianness 
void MarshalUint32(ostream &out, const uint32_t val) {
  char buf[4];
  memcpy(buf, &val, 4);
  out.write(buf, 4);
}
void UnmarshalUint32(uint32_t* val, istream &in) {
  char buf[4];
  in.read(buf, 4);
  memcpy(val, buf, 4);
}

void MarshalString(ostream &out, const string &str) {
  if (str.size() > UINT32_MAX)
    throw out_of_range("string longer than UINT32_MAX");

  MarshalUint32(out, str.size());
  // Both solutions are equivalent:
  // out.write(&str.at(0), str.size());
  out << str;

}
void UnmarshalString(string* str, istream &in) {
  uint32_t size;
  UnmarshalUint32(&size, in);

  str->resize(size);
  in.read(&str->at(0), size);

  string str2(*str);
}

// String representation
// RFC3339: "2006-01-02T15:04:05Z07:00"
// Nothing is put/read after the 'Z'
void MarshalTime(std::ostream &out, const time_t &val) {
  char buf[20] = {0};
  strftime(buf, 20, "%Y-%m-%dT%H:%M:%SZ", localtime(&val));
  out.write(buf, 20);
  // std::cout << "Marshaled: " << buf << std::endl;

  // Binary representation: works (remove tm_yday and tm_wday)
  // struct tm* t = localtime(&val);
  // char buf[9];
  // buf[0] = (unsigned char) t->tm_isdst;
  // // buf[1] = (unsigned char) t->tm_yday;
  // // buf[2] = (unsigned char) t->tm_wday;
  // buf[3] = (unsigned char) t->tm_year;
  // buf[4] = (unsigned char) t->tm_mon;
  // buf[5] = (unsigned char) t->tm_mday;
  // buf[6] = (unsigned char) t->tm_hour;
  // buf[7] = (unsigned char) t->tm_min;
  // buf[8] = (unsigned char) t->tm_sec;
  // out.write(buf, 9);
}
void UnmarshalTime(time_t* val, std::istream &in) {
  struct tm t;
  char buf[20];
  char* p = buf;
  in.read(buf, 20);
  // cout << "Buffer content: " << buf << endl;

  // Daylight saving time -- dunno
  t.tm_isdst = -1; // -1 for unavailable
  // YYYY
  t.tm_year = atoi(p) - 1900;
  p += 5;
  // MM
  t.tm_mon = atoi(p) - 1;
  p += 3;
  // DD
  t.tm_mday = atoi(p);
  p += 3;
  // HH
  t.tm_hour = atoi(p);
  p += 3;
  // mm
  t.tm_min = atoi(p);
  p += 3;
  // ss
  t.tm_sec = atoi(p); 

  *val = mktime(&t);

  // debug
  // strftime(buf, 20, "%Y-%m-%dT%H:%M:%SZ", localtime(val));
  // std::cout << "Unmarshaled: " << buf << std::endl;

  // Binary representation: works (remove tm_yday and tm_wday)
  // struct tm t;
  // char buf[9];
  // in.read(buf, 9);
  // t.tm_isdst = buf[0];
  // // t.tm_yday = buf[1];
  // // t.tm_wday = buf[2];
  // t.tm_year = (unsigned char) buf[3];
  // t.tm_mon = (unsigned char) buf[4];
  // t.tm_mday = (unsigned char) buf[5];
  // t.tm_hour = (unsigned char) buf[6];
  // t.tm_min = (unsigned char) buf[7];
  // t.tm_sec = (unsigned char) buf[8];
  // *val = mktime(&t);
}