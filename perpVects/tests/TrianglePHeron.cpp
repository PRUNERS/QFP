#include "Triangle.hpp"
#include "QFPHelpers.h"

#include <cmath>

template <typename T>
class TrianglePHeron: public Triangle<T> {
public:
  TrianglePHeron(std::string id):Triangle<T>(id){}

  //computes using Heron's formula
  T getArea(const T a,
            const T b,
            const T c){
    T s = (a + b + c ) / 2;
    return sqrt(s* (s-a) * (s-b) * (s-c));
  }
};

REGISTER_TYPE(TrianglePHeron)