#include "View.hpp"

#include <sstream>
#include <algorithm> 
#include <vector>

#include "marshaling.hpp"

using namespace std;

std::shared_ptr<View> View::FromStream(std::istream& in) {
  auto v = std::make_shared<View>();
  v->Unmarshal(in);
  return v;
}

// Format: <nDescs>[<Desc>...]
void View::Marshal(std::ostream& out) const {
  MarshalUint32(out, mDescs.size());
  for (auto& it: mDescs) {
    it.second->Marshal(out);
  }
}
void View::Unmarshal(std::istream& in) {
  unsigned int nDescs;
  UnmarshalUint32(&nDescs, in);
  for (unsigned int i = 0; i < nDescs; i++) {
    Add(*Descriptor::FromStream(in));
  }
}

bool View::DeepEquals(const View& v) const {
  auto vMap = v.Map();
  if (mDescs.size() != v.Map().size())
    return false;

  for (auto& it: mDescs) {
    auto vIt = vMap.find(it.second->ID());

    if (vIt == vMap.end() || 
       !vIt->second->DeepEquals(*it.second)) 
      return false;
  }
  return true;
}

void View::Add(const Descriptor& d) {
  // Only add if unfound or d younger than existing
  auto existing = mDescs.find(d.ID());
  if (existing == mDescs.end() || 
      d.Younger(*existing->second)) {
    mDescs[d.ID()] = make_shared<Descriptor>(d);
  }
}

void View::Remove(const string &key) {
  mDescs.erase(key);
}

void View::Diff(const View& v) {
  for (auto& it : v.mDescs) {
    mDescs.erase(it.first);
  }
}

void View::IncrementAge() {
  for (auto& it : mDescs) {
    it.second->IncrementAge();
  }
}

std::shared_ptr<View> View::RandomSubset(const unsigned int n) {
  auto v = std::make_shared<View>();

  if (mDescs.size() <= n) {
    v->mDescs = mDescs;
    return v;
  }

  // Make a sequence of the map keys
  std::vector<std::string> perm;
  perm.reserve(mDescs.size());
  for (auto it: mDescs)
    perm.push_back(it.first);
  // Shuffle sequence and resize to n elements
  std::random_shuffle ( perm.begin(), perm.end() );
  perm.resize(n);

  v->mDescs.reserve(n);
  for (unsigned int i = 0; i < perm.size(); i++) {
    //auto d = mDescs.at(perm[i]).second;
    v->mDescs[perm[i]] = mDescs[perm[i]];
  }

  return v;
}

std::shared_ptr<Descriptor> View::Oldest() {
  std::shared_ptr<Descriptor> d;

  unsigned int maxAge = 0;

  for (auto& it: mDescs) {
    if (it.second->Age() > maxAge) {
      maxAge = it.second->Age();
      d = it.second;
    }
  }
  return d;
}

string View::String() {
  int len = mDescs.size();

  if (len == 0) { return "View(len=0){}"; }

  stringstream ss;
  ss << "View(len=" << len << "){\n";
  for (auto& it : mDescs) {
    ss << "\t" << it.second->String() << "\n";
  }
  ss << "}";

  return ss.str();
}