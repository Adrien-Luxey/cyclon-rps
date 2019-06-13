#include <catch.hpp>
#include <sstream>

#include "Descriptor.hpp"

TEST_CASE( "Un/marshaling Descriptor using name constructor", "[marshal][Descriptor]" ) {
  Descriptor input("123.147.23.87", "dousqoiuqsdiouzzz");
  input.IncrementAge();
  input.IncrementAge();
  input.IncrementAge();
  input.IncrementAge();

  std::stringstream ss;

  input.Marshal(ss);

  auto output = Descriptor::FromStream(ss);

  REQUIRE( input.DeepEquals(*output) );
  REQUIRE( ss.tellp() == ss.tellg() );
  
  //delete(output);
}