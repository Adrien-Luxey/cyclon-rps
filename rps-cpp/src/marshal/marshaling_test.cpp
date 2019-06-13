#include <catch.hpp>

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>

#include "marshaling.hpp"
#include "Descriptor.hpp"

using namespace std;

TEST_CASE( "Un/marshaling Bools with stringstream", "[marshal][bool]" ) {
  bool falseInput = false, falseOutput;
  bool trueInput = true, trueOutput;
  stringstream ss;

  MarshalBool(ss, trueInput);
  MarshalBool(ss, falseInput);
  UnmarshalBool(&trueOutput, ss);
  UnmarshalBool(&falseOutput, ss);

  REQUIRE( trueInput == trueOutput );
  REQUIRE( falseInput == falseOutput );
  REQUIRE( ss.tellp() == ss.tellg() );
}

TEST_CASE( "Un/marshaling Uint32 with stringstream", "[marshal][uint32]" ) {
  uint32_t input = 123083, output;
  stringstream ss;

  MarshalUint32(ss, input);
  //cout << "Stream (tellp: " << ss.tellp() << "): '" << ss.str() << "'" << endl;
  UnmarshalUint32(&output, ss);

  REQUIRE( input == output );
  REQUIRE( ss.tellp() == ss.tellg() );
}

TEST_CASE( "Un/marshaling String with stringstream", "[marshal][string]" ) {
  string input, output;
  stringstream ss;

  for (int i = 0; i < 10; i++) {
    input += "lalkqdhlkaluzkqsdjlldkj ";
  }

  MarshalString(ss, input);
  //cout << "Stream (tellp: " << ss.tellp() << "): '" << ss.str() << "'" << endl;
  UnmarshalString(&output, ss);

  REQUIRE( input == output );
  REQUIRE( ss.tellp() == ss.tellg() );
}

TEST_CASE( "Un/marshaling Time with stringstream", "[marshal][time]" ) {
  time_t input = time(nullptr), output;
  stringstream ss;

  MarshalTime(ss, input);
  UnmarshalTime(&output, ss);

  REQUIRE( input == output );
  REQUIRE( ss.tellp() == ss.tellg() );
}

TEST_CASE( "Un/marshaling consecutive types with stringstream", "[marshal][consecutive]" ) {
  string strInput = "lorem ipsum sit dolor amen blah blah", strOutput;
  bool trueInput = true, trueOutput;
  string str2Input = "OMAGAD OMAGAD OMAGAD!!!1!1!\n\0", str2Output;
  uint32_t uiInput = 12387, uiOutput;
  Descriptor descInput("123.147.23.87", "youpi youpla");
  Descriptor descOutput;
  stringstream ss;

  MarshalString(ss, strInput);
  MarshalBool(ss, trueInput);
  MarshalString(ss, str2Input);
  MarshalUint32(ss, uiInput);
  descInput.Marshal(ss);
  // cout << "Stream (tellp: " << ss.tellp() << "): '" << ss.str() << "'" << endl;
  UnmarshalString(&strOutput, ss);
  UnmarshalBool(&trueOutput, ss);
  UnmarshalString(&str2Output, ss);
  UnmarshalUint32(&uiOutput, ss);
  descOutput.Unmarshal(ss);

  REQUIRE( strInput == strOutput );
  REQUIRE( trueInput == trueOutput );
  REQUIRE( str2Input == str2Output );
  REQUIRE( uiInput == uiOutput );
  REQUIRE( descInput.DeepEquals(descOutput) );
  REQUIRE( ss.tellp() == ss.tellg() );
}