#ifndef RPS_MESSAGE_
#define RPS_MESSAGE_

#include <memory>
#include <iostream>

#include "Descriptor.hpp"
#include "View.hpp"


struct Message {
 public:
  // Message() {}
  // Message(const Descriptor& d, const View& v)
  //   : Sender(std::make_shared<Descriptor>(d)), V(std::make_shared<View>(v)) {}
  static std::shared_ptr<Message> FromStream(std::istream& in);
  // - Un/Marshaling
  void Marshal(std::ostream& out) const;
  void Unmarshal(std::istream& in);

  // - Content
  std::shared_ptr<Descriptor> Sender;
  std::shared_ptr<View> V;
};

#endif // RPS_MESSAGE_