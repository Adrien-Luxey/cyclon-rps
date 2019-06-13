#include <catch.hpp>
#include <sstream>

#include "rand_string.hpp"
//#include "Descriptor.hpp"
#include "View.hpp"

TEST_CASE( "Un/marshaling View using named constructor", "[marshal][View]" ) {
  const int N_DESCS = 100;

  View input;
  for (int i = 0; i < N_DESCS; i++) {
    std::string s = randString(10);
    input.Add(Descriptor(s, s));
  }
  input.IncrementAge();
  std::stringstream ss;

  // std::cout << "Input view: " << input.String() << std::endl;

  input.Marshal(ss);

  auto output = View::FromStream(ss);

  REQUIRE( input.DeepEquals(*output) );
  REQUIRE( ss.tellp() == ss.tellg() );
}