#ifndef RPS_DESCRIPTOR_HPP_
#define RPS_DESCRIPTOR_HPP_

#include <string>
#include <ctime>
#include <cstdint>
#include <iostream>
#include <memory>


class Descriptor {
 public:
  Descriptor() 
    : mIP(), mID(), mAge(0), mTimestamp(0) {}
  Descriptor(const std::string IP, const std::string ID)
    : mIP(IP), mID(ID), mAge(0), mTimestamp(time(nullptr)) {}

  // https://isocpp.org/wiki/faq/ctors#named-ctor-idiom
  static std::shared_ptr<Descriptor> FromStream(std::istream& in);

  // No copy
  // Descriptor(const Descriptor&) = delete;
  // Descriptor& operator=(const Descriptor&) = delete;

  // - Un/Marshaling
  void Marshal(std::ostream& out) const;
  void Unmarshal(std::istream& in);

  // - Stringer
  std::string String() const;

  bool DeepEquals(const Descriptor& d) const;

  // - Accessors 
  const std::string& IP() const { return mIP; }
  const std::string& ID() const { return mID; }
  const uint32_t& Age() const { return mAge; }
  const std::time_t& Timestamp() const { return mTimestamp; }
  // -- Age
  bool Younger(const Descriptor& d) const { return mAge <= d.Age(); }
  void IncrementAge() { mAge += 1; }
  // -- Timestamp
  void UpdateTimestamp() { mTimestamp = std::time(nullptr); }


 private:
  std::string mIP, mID;
  uint32_t mAge;

  // https://en.cppreference.com/w/c/chrono/time_t
  // second precision (is it enough?)
  std::time_t mTimestamp;
};

#endif // RPS_DESCRIPTOR_HPP_