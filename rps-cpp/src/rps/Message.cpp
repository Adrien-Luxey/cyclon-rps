#include "Message.hpp"

#include <stdexcept>

#include "error.hpp"

std::shared_ptr<Message> Message::FromStream(std::istream& in) {
  auto m = std::make_shared<Message>();
  m->Unmarshal(in);
  return m;
}

void Message::Marshal(std::ostream& out) const {
  if (!Sender)
    throw std::runtime_error(
      buildErrorMessage("Message::", __func__, ": Empty Sender"));
  if (!V)
    throw std::runtime_error(
      buildErrorMessage("Message::", __func__, ": Empty V"));

  Sender->Marshal(out);
  V->Marshal(out);
}
void Message::Unmarshal(std::istream& in) {
  Sender = Descriptor::FromStream(in);
  V = View::FromStream(in);
}