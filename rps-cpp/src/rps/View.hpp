#ifndef RPS_VIEW_HPP_
#define RPS_VIEW_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include "Descriptor.hpp"

class View {
 public:
  View() {}
  static std::shared_ptr<View> FromStream(std::istream& in);

  // - Un/Marshaling
  void Marshal(std::ostream& out) const;
  void Unmarshal(std::istream& in);

  bool DeepEquals(const View& v) const;

  // - Modifiers
  void Add(const Descriptor& d);
  void Remove(const std::string& key);
  void Diff(const View& v);
  void IncrementAge();

  // - Accessors
  // Returns a new View containing a subset of n descriptors' smart pointers
  std::shared_ptr<View> RandomSubset(const unsigned int n);
  std::shared_ptr<Descriptor> Oldest();
  const std::unordered_map<std::string, std::shared_ptr<Descriptor>> 
    Map() const { return mDescs; }
  unsigned int Size() const { return mDescs.size(); }


  // - Stringer
  std::string String();

 private:
  // Key is Descriptor's ID
  std::unordered_map<std::string, std::shared_ptr<Descriptor>> mDescs;
};

#endif // RPS_VIEW_HPP_
