#include <catch.hpp>
#include <sstream>

// #include "Descriptor.hpp"
// #include "View.hpp"
#include "Message.hpp"
#include "rand_string.hpp"

TEST_CASE( "Un/marshaling Message using name constructor", "[marshal][Message]" ) {
  const int N_DESCS = 10;

  Message input = Message{
    Sender: std::make_shared<Descriptor>("123.147.23.87", "PADAKOR\n"),
    V: std::make_shared<View>()
  };
  for (int i = 0; i < N_DESCS; i++) {
    std::string s = randString(10);
    input.V->Add(Descriptor(s, s));
    // Increment the age of the descriptor while we're at it
    input.Sender->IncrementAge();
  }

  std::stringstream ss;

  input.Marshal(ss);
  auto output = Message::FromStream(ss);


  REQUIRE( input.Sender->DeepEquals(*output->Sender) );
  REQUIRE( input.V->DeepEquals(*output->V) );
  REQUIRE( ss.tellp() == ss.tellg() );
  
  //delete(output);
}