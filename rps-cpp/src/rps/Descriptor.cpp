#include "Descriptor.hpp"

#include <sstream> 

#include "marshaling.hpp"

using namespace std;


std::shared_ptr<Descriptor> Descriptor::FromStream(istream& in) {
  auto d = std::make_shared<Descriptor>();
  d->Unmarshal(in);
  return d;
}

// Format:
// <IP><ID><Age><Timestamp>
void Descriptor::Marshal(ostream &out) const {
  MarshalString(out, mIP);
  MarshalString(out, mID);
  MarshalUint32(out, mAge);
  MarshalTime(out, mTimestamp);
}

void Descriptor::Unmarshal(istream& in) {
  UnmarshalString(&mIP, in);
  UnmarshalString(&mID, in);
  UnmarshalUint32(&mAge, in);
  UnmarshalTime(&mTimestamp, in);
}


string Descriptor::String() const {
  stringstream ss;
  char tStr[20] = {0};
  strftime(tStr, 20, "%Y-%m-%dT%H:%M:%SZ", localtime(&mTimestamp));
  
  ss << "Descriptor{ ID: " << mID << ", IP: " << mIP << \
    ", Age: " << mAge << ", Ts: " << tStr << " }";
  return ss.str();
}

bool Descriptor::DeepEquals(const Descriptor& d) const {
  return mIP == d.IP() &&
    mID == d.ID() &&
    mAge == d.Age() &&
    mTimestamp == d.Timestamp();

}