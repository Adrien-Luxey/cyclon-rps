#ifndef MARSHAL_MARSHALING_HPP_
#define MARSHAL_MARSHALING_HPP_

#include <string>
#include <iostream>
#include <cstdint>
#include <ctime>

// Advice on reading/writing simple types in binary format:
// https://isocpp.org/wiki/faq/serialization
// https://isocpp.org/wiki/faq/serialization#serialize-binary-format
// https://isocpp.org/wiki/faq/serialization#serialize-no-cycles-no-joins
// https://isocpp.org/wiki/faq/serialization#serialize-inherit-no-ptrs

void MarshalBool(std::ostream &out, const bool val);
void UnmarshalBool(bool* val, std::istream &in);

void MarshalUint32(std::ostream &out, const uint32_t val);
void UnmarshalUint32(uint32_t* val, std::istream &in);

void MarshalString(std::ostream &out, const std::string &str);
void UnmarshalString(std::string* str, std::istream &in);

void MarshalTime(std::ostream &out, const time_t &val);
void UnmarshalTime(time_t* val, std::istream &in);

// void MarshalDouble(std::ostream &out, const double val);
// void UnmarshalDouble(double* val, std::istream &in);

#endif // MARSHAL_MARSHALING_HPP_