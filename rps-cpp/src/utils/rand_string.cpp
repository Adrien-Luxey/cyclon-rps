#include "rand_string.hpp"

std::string randString(int n) { 
  std::string res, alphabet = "abcdefghijklmnopqrstuvwxyz0123456789";
  res.resize(n);
  for (int i = 0; i < n; i++) 
    res[i] = alphabet[rand() % alphabet.size()]; 
  return res; 
}