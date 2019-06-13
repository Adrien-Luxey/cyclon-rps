#include <catch.hpp>
#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include <thread>

#include "TCPListener.hpp"
#include "TCPConnection.hpp"
#include "Descriptor.hpp"
#include "marshaling.hpp"

// TODO: test two-way communication

TEST_CASE("Exchanging big string through TCPConnection with sputn/sgetn", 
          "[net][TCPConnection][string]" ) {
  const int port = 111111;
  std::string inputClient, outputServer;
  std::string inputServer, outputClient;

  // Crafting a bigger string
  for (int i = 0; i < 400; i++) {
    inputClient += "Gabuzomeu is a cry for love baby ";
    inputServer += "If only my waffles were bigger ";
  }

  const std::streamsize clientToServBufSize = inputClient.size();
  const std::streamsize servToClientBufSize = inputServer.size();

  // Server thread executes following lambda
  std::thread server_thread([&]() {
    std::shared_ptr<TCPListener> l;
    try {
      l = std::make_shared<TCPListener>(port);
    } catch (std::exception &e) {
      std::cerr << "Error creating TCPListener: " << e.what() << std::endl;
      return;
    }

    std::shared_ptr<TCPConnection> conn;
    try {
      conn = l->Accept();
    } catch (std::exception &e) {
      std::cerr << "Error calling TCPListener::Accept: " << e.what() << std::endl;
      return;
    }

    char buf[clientToServBufSize];
    std::streamsize nRcvd;
    try {
      nRcvd = conn->sgetn(buf, clientToServBufSize);
    } catch (std::exception& e) {
      std::cerr << "Error calling TCPConnection::sgetn from server: " << e.what() << std::endl;
      return;
    }
    outputServer.append(buf, nRcvd);

    try {
      conn->sputn(&inputServer.at(0), inputServer.size());
    } catch (std::exception& e) {
      std::cerr << "Error calling TCPConnection::sputn from server: " << e.what() << std::endl;
      return;
    }
  });

  // Wait a bit for the listener to be created
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  std::shared_ptr<TCPConnection> client;
  try {
    client = TCPConnection::Dial(std::string("127.0.0.1"), port);
  } catch (std::exception &e) {
    std::cerr << "Error creating TCPConnection: " << e.what() << std::endl;
    return;
  }

  try {
    client->sputn(&inputClient.at(0), inputClient.size());
  } catch (std::exception &e) {
    std::cerr << "Error callingTCPConnection::sputn from client: " << e.what() << std::endl;
    return;
  }

  char buf[servToClientBufSize];
  std::streamsize nRcvd;
  try {
    nRcvd = client->sgetn(buf, servToClientBufSize);
  } catch (std::exception& e) {
    std::cerr << "Error calling TCPConnection::sgetn from client: " << e.what() << std::endl;
    return;
  }
  outputClient.append(buf, nRcvd);

  try {
    client->Close();
  } catch (std::exception& e) {
    std::cerr << "Error calling TCPConnection::Clise: " << e.what() << std::endl;
    return;
  }

  server_thread.join();

  REQUIRE( inputClient == outputServer );
  REQUIRE( inputServer == outputClient );
}

TEST_CASE("Exchanging consecutive marshaled types through TCPConnection", 
          "[net][TCPConnection][marshal]") {
  const int port = 111112;

  std::string strInput = "lorem ipsum sit dolor amen blah blah", strOutput;
  bool trueInput = true, trueOutput;
  std::string str2Input = "OMAGAD OMAGAD OMAGAD!!!1!1!\n\0", str2Output;
  uint32_t uiInput = 12387, uiOutput;
  Descriptor descInput("123.147.23.88", "MyIDIsBetterThanYours");
  Descriptor descOutput;


  // Server thread executes following lambda
  std::thread server_thread([&]() {
    std::shared_ptr<TCPListener> l;
    try {
      l = std::make_shared<TCPListener>(port);
    } catch (std::exception &e) {
      std::cerr << "Error creating TCPListener: " << e.what() << std::endl;
      return;
    }

    std::shared_ptr<TCPConnection> conn;
    try {
      conn = l->Accept();
    } catch (std::exception &e) {
      std::cerr << "Error calling TCPListener::Accept: " << e.what() << std::endl;
      // delete(l);
      return;
    }

    std::ostream os(conn.get());
    try {
      MarshalString(os, strInput);
      MarshalBool(os, trueInput);
      MarshalString(os, str2Input);
      MarshalUint32(os, uiInput);
      descInput.Marshal(os);
    } catch (std::exception& e) {
      std::cerr << "Error calling TCPConnection::sgetn: " << e.what() << std::endl;
      // delete(conn);
      // delete(l);
      return;
    }

    // delete(conn);
    // delete(l);

  });

  // Wait a bit for the listener to be created
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::shared_ptr<TCPConnection> client;
  try {
    client = TCPConnection::Dial(std::string("127.0.0.1"), port);
  } catch (std::exception &e) {
    std::cerr << "Error creating TCPConnection: " << e.what() << std::endl;
    return;
  }

  std::istream is(client.get());
  try {
    UnmarshalString(&strOutput, is);
    UnmarshalBool(&trueOutput, is);
    UnmarshalString(&str2Output, is);
    UnmarshalUint32(&uiOutput, is);
    descOutput.Unmarshal(is);
    client->Close();
  } catch (std::exception &e) {
    std::cerr << "Error callingTCPConnection::sputn: " << e.what() << std::endl;
    // delete(client);
    return;
  }

  server_thread.join();
  // delete(client);

  REQUIRE( strInput == strOutput );
  REQUIRE( trueInput == trueOutput );
  REQUIRE( str2Input == str2Output );
  REQUIRE( uiInput == uiOutput );
  REQUIRE( descInput.DeepEquals(descOutput) );
}