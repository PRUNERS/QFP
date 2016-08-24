// this contains the helper classes for the tests.
// they utilize the watch data for sensitive points
// of computation.

#include "QFPHelpers.h"

#include <iostream>
#include <unordered_map>
#include <mutex>

namespace QFPHelpers {

void printOnce(std::string s, void* addr){
  return;
  static std::unordered_map<void*, std::string> seen;
  if(seen.count(addr) == 0){
    seen.insert({addr, s});
    std::cout << s << " at: " << addr << std::endl;
    asm("int $3");
  }
}

thread_local InfoStream info_stream;
std::mutex ostreamMutex;

std::ostream& operator<<(std::ostream& os, const unsigned __int128 i) {
  uint64_t hi = i >> 64;
  uint64_t lo = (uint64_t)i;
  os << hi << lo;
  return os;
}

std::ostream& operator<<(std::ostream& os, const unsigned __int128 &i){
  uint64_t hi = i >> 64;
  uint64_t lo = (uint64_t)i;
  os << hi << lo;
  return os;
}

namespace FPHelpers{
    auto
  swap_float_int(unsigned int val) -> float{
    return *reinterpret_cast<float*>(&val);
  }

  auto
  swap_float_int(unsigned long val) -> double{
     return *reinterpret_cast<double*>(&val);
  }

  auto
  swap_float_int(unsigned __int128 val) -> long double{
    return *reinterpret_cast<long double*>(&val);
  }

  auto
  swap_float_int(float val) -> unsigned int{
    return *reinterpret_cast<unsigned int*>(&val);
  }

  auto
  swap_float_int(double val) -> unsigned long{
    return *reinterpret_cast<unsigned long*>(&val);
  }

  auto
  swap_float_int(long double val) -> unsigned __int128{
    return *reinterpret_cast<unsigned __int128*>(&val);
  }
}

// std::string
// getSortName(sort_t val){
//   switch(val){
//   case lt:
//     return "lt";
//   case gt:
//     return "gt";
//   case bi:
//     return "bi";
//   case def:
//     return "us";
//   default:
//     return "something bad happened, undefined sort type";
//   }
// }
}
