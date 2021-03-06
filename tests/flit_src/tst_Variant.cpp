/* -- LICENSE BEGIN --
 *
 * Copyright (c) 2015-2020, Lawrence Livermore National Security, LLC.
 *
 * Produced at the Lawrence Livermore National Laboratory
 *
 * Written by
 *   Michael Bentley (mikebentley15@gmail.com),
 *   Geof Sawaya (fredricflinstone@gmail.com),
 *   and Ian Briggs (ian.briggs@utah.edu)
 * under the direction of
 *   Ganesh Gopalakrishnan
 *   and Dong H. Ahn.
 *
 * LLNL-CODE-743137
 *
 * All rights reserved.
 *
 * This file is part of FLiT. For details, see
 *   https://pruners.github.io/flit
 * Please also read
 *   https://github.com/PRUNERS/FLiT/blob/master/LICENSE
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the disclaimer below.
 *
 * - Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the disclaimer
 *   (as noted below) in the documentation and/or other materials
 *   provided with the distribution.
 *
 * - Neither the name of the LLNS/LLNL nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL
 * SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Additional BSD Notice
 *
 * 1. This notice is required to be provided under our contract
 *    with the U.S. Department of Energy (DOE). This work was
 *    produced at Lawrence Livermore National Laboratory under
 *    Contract No. DE-AC52-07NA27344 with the DOE.
 *
 * 2. Neither the United States Government nor Lawrence Livermore
 *    National Security, LLC nor any of their employees, makes any
 *    warranty, express or implied, or assumes any liability or
 *    responsibility for the accuracy, completeness, or usefulness of
 *    any information, apparatus, product, or process disclosed, or
 *    represents that its use would not infringe privately-owned
 *    rights.
 *
 * 3. Also, reference herein to any specific commercial products,
 *    process, or services by trade name, trademark, manufacturer or
 *    otherwise does not necessarily constitute or imply its
 *    endorsement, recommendation, or favoring by the United States
 *    Government or Lawrence Livermore National Security, LLC. The
 *    views and opinions of authors expressed herein do not
 *    necessarily state or reflect those of the United States
 *    Government or Lawrence Livermore National Security, LLC, and
 *    shall not be used for advertising or product endorsement
 *    purposes.
 *
 * -- LICENSE END --
 */

#include "test_harness.h"

#include <flit/Variant.h>
#include <flit/Variant.cpp>

#include <sstream>
#include <string>
#include <vector>

namespace {

std::ostream& operator<<(std::ostream& out, std::vector<std::string> vec) {
  bool first = true;
  out << "[";
  for (std::string &val : vec) {
    if (!first) {
      out << ", ";
    }
    first = false;
    out << '"' << val << '"';
  }
  out << "]";
  return out;
}

TH_TEST(tst_vector_stream_operator) {
  std::ostringstream out;
  out << std::vector<std::string> {"a", "b", "cde"};
  TH_EQUAL("[\"a\", \"b\", \"cde\"]", out.str());
}

} // end of unnamed namespace

TH_TEST(tst_Variant_emptyConstructor) {
  flit::Variant v;
  TH_EQUAL(v.type(), flit::Variant::Type::None);
  TH_THROWS(v.longDouble(), std::runtime_error);
  TH_THROWS(v.string(), std::runtime_error);
  TH_THROWS(v.vectorString(), std::runtime_error);
  TH_THROWS(v.vectorFloat(), std::runtime_error);
  TH_THROWS(v.vectorDouble(), std::runtime_error);
  TH_THROWS(v.vectorLongDouble(), std::runtime_error);
}

TH_TEST(tst_Variant_longDoubleConstructor) {
  long double value = 5.4L;
  flit::Variant v(value);
  TH_EQUAL(v.type(), flit::Variant::Type::LongDouble);
  TH_EQUAL(v.longDouble(), value);
  TH_THROWS(v.string(), std::runtime_error);
  TH_THROWS(v.vectorString(), std::runtime_error);
  TH_THROWS(v.vectorFloat(), std::runtime_error);
  TH_THROWS(v.vectorDouble(), std::runtime_error);
  TH_THROWS(v.vectorLongDouble(), std::runtime_error);
}

TH_TEST(tst_Variant_stringConstructor_reference) {
  std::string value("hello there");
  flit::Variant v(value);
  TH_EQUAL(v.type(), flit::Variant::Type::String);
  TH_THROWS(v.longDouble(), std::runtime_error);
  TH_EQUAL(v.string(), value);
  TH_THROWS(v.vectorString(), std::runtime_error);
  TH_THROWS(v.vectorFloat(), std::runtime_error);
  TH_THROWS(v.vectorDouble(), std::runtime_error);
  TH_THROWS(v.vectorLongDouble(), std::runtime_error);
}

TH_TEST(tst_Variant_stringConstructor_rvalueReference) {
  std::string value("hello there");
  std::string copy(value);
  flit::Variant v(std::move(copy));
  TH_EQUAL(v.type(), flit::Variant::Type::String);
  TH_THROWS(v.longDouble(), std::runtime_error);
  TH_EQUAL(v.string(), value);
  TH_THROWS(v.vectorString(), std::runtime_error);
  TH_THROWS(v.vectorFloat(), std::runtime_error);
  TH_THROWS(v.vectorDouble(), std::runtime_error);
  TH_THROWS(v.vectorLongDouble(), std::runtime_error);
}

TH_TEST(tst_Variant_stringConstructor_cstring) {
  std::string value("hello there");
  flit::Variant v(value.c_str());
  TH_EQUAL(v.type(), flit::Variant::Type::String);
  TH_THROWS(v.longDouble(), std::runtime_error);
  TH_EQUAL(v.string(), value);
  TH_THROWS(v.vectorString(), std::runtime_error);
  TH_THROWS(v.vectorFloat(), std::runtime_error);
  TH_THROWS(v.vectorDouble(), std::runtime_error);
  TH_THROWS(v.vectorLongDouble(), std::runtime_error);
}

TH_TEST(tst_Variant_vectorStringConstructor_reference) {
  std::vector<std::string> value { "a", "b", "cde", "fgh" };
  flit::Variant v(value);
  TH_EQUAL(v.type(), flit::Variant::Type::VectorString);
  TH_THROWS(v.longDouble(), std::runtime_error);
  TH_THROWS(v.string(), std::runtime_error);
  TH_EQUAL(v.vectorString(), value);
  TH_THROWS(v.vectorFloat(), std::runtime_error);
  TH_THROWS(v.vectorDouble(), std::runtime_error);
  TH_THROWS(v.vectorLongDouble(), std::runtime_error);
}

TH_TEST(tst_Variant_vectorStringConstructor_rvalueReference) {
  std::vector<std::string> value { "a", "b", "cde", "fgh" };
  std::vector<std::string> copy(value);
  flit::Variant v(std::move(copy));
  TH_EQUAL(v.type(), flit::Variant::Type::VectorString);
  TH_THROWS(v.longDouble(), std::runtime_error);
  TH_THROWS(v.string(), std::runtime_error);
  TH_EQUAL(v.vectorString(), value);
  TH_THROWS(v.vectorFloat(), std::runtime_error);
  TH_THROWS(v.vectorDouble(), std::runtime_error);
  TH_THROWS(v.vectorLongDouble(), std::runtime_error);
}

TH_TEST(tst_Variant_vectorFloatConstructor_reference) {
  std::vector<float> value { 3.2f, 1.4f, 5.4f };
  flit::Variant v(value);
  TH_EQUAL(v.type(), flit::Variant::Type::VectorFloat);
  TH_THROWS(v.longDouble(), std::runtime_error);
  TH_THROWS(v.string(), std::runtime_error);
  TH_THROWS(v.vectorString(), std::runtime_error);
  TH_EQUAL(v.vectorFloat(), value);
  TH_THROWS(v.vectorDouble(), std::runtime_error);
  TH_THROWS(v.vectorLongDouble(), std::runtime_error);
}

TH_TEST(tst_Variant_vectorFloatConstructor_rvalueReference) {
  std::vector<float> value { 3.2f, 1.4f, 5.4f };
  std::vector<float> copy(value);
  flit::Variant v(std::move(copy));
  TH_EQUAL(v.type(), flit::Variant::Type::VectorFloat);
  TH_THROWS(v.longDouble(), std::runtime_error);
  TH_THROWS(v.string(), std::runtime_error);
  TH_THROWS(v.vectorString(), std::runtime_error);
  TH_EQUAL(v.vectorFloat(), value);
  TH_THROWS(v.vectorDouble(), std::runtime_error);
  TH_THROWS(v.vectorLongDouble(), std::runtime_error);
}

TH_TEST(tst_Variant_vectorDoubleConstructor_reference) {
  std::vector<double> value { 3.14159, 14.883, .54321, 737373. };
  flit::Variant v(value);
  TH_EQUAL(v.type(), flit::Variant::Type::VectorDouble);
  TH_THROWS(v.longDouble(), std::runtime_error);
  TH_THROWS(v.string(), std::runtime_error);
  TH_THROWS(v.vectorString(), std::runtime_error);
  TH_THROWS(v.vectorFloat(), std::runtime_error);
  TH_EQUAL(v.vectorDouble(), value);
  TH_THROWS(v.vectorLongDouble(), std::runtime_error);
}

TH_TEST(tst_Variant_vectorDoubleConstructor_rvalueReference) {
  std::vector<double> value { 3.14159, 14.883, .54321, 737373. };
  std::vector<double> copy(value);
  flit::Variant v(std::move(copy));
  TH_EQUAL(v.type(), flit::Variant::Type::VectorDouble);
  TH_THROWS(v.longDouble(), std::runtime_error);
  TH_THROWS(v.string(), std::runtime_error);
  TH_THROWS(v.vectorString(), std::runtime_error);
  TH_THROWS(v.vectorFloat(), std::runtime_error);
  TH_EQUAL(v.vectorDouble(), value);
  TH_THROWS(v.vectorLongDouble(), std::runtime_error);
}

TH_TEST(tst_Variant_vectorLongDoubleConstructor_reference) {
  std::vector<long double> value { 3.14159L, 14.883L, .54321L, 737373.L };
  flit::Variant v(value);
  TH_EQUAL(v.type(), flit::Variant::Type::VectorLongDouble);
  TH_THROWS(v.longDouble(), std::runtime_error);
  TH_THROWS(v.string(), std::runtime_error);
  TH_THROWS(v.vectorString(), std::runtime_error);
  TH_THROWS(v.vectorFloat(), std::runtime_error);
  TH_THROWS(v.vectorDouble(), std::runtime_error);
  TH_EQUAL(v.vectorLongDouble(), value);
}

TH_TEST(tst_Variant_vectorLongDoubleConstructor_rvalueReference) {
  std::vector<long double> value { 3.14159L, 14.883L, .54321L, 737373.L };
  std::vector<long double> copy(value);
  flit::Variant v(std::move(copy));
  TH_EQUAL(v.type(), flit::Variant::Type::VectorLongDouble);
  TH_THROWS(v.longDouble(), std::runtime_error);
  TH_THROWS(v.string(), std::runtime_error);
  TH_THROWS(v.vectorString(), std::runtime_error);
  TH_THROWS(v.vectorFloat(), std::runtime_error);
  TH_THROWS(v.vectorDouble(), std::runtime_error);
  TH_EQUAL(v.vectorLongDouble(), value);
}

TH_TEST(tst_Variant_copyConstructor) {
  flit::Variant v1;
  flit::Variant v2(3.14159);
  flit::Variant v3("hello there");
  flit::Variant v4(std::vector<std::string> { "a", "b", "cde", "fgh" });
  flit::Variant v5(std::vector<float> { 314159.f });
  flit::Variant v6(std::vector<double> { 3.14159e-5 });
  flit::Variant v7(std::vector<long double> { 4452346L });

  flit::Variant c1(v1);
  flit::Variant c2(v2);
  flit::Variant c3(v3);
  flit::Variant c4(v4);
  flit::Variant c5(v5);
  flit::Variant c6(v6);
  flit::Variant c7(v7);

  TH_EQUAL(v1, c1);
  TH_EQUAL(v2, c2);
  TH_EQUAL(v3, c3);
  TH_EQUAL(v4, c4);
  TH_EQUAL(v5, c5);
  TH_EQUAL(v6, c6);
  TH_EQUAL(v7, c7);
}

TH_TEST(tst_Variant_moveConstructor) {
  flit::Variant v1;
  flit::Variant v2(3.14159);
  flit::Variant v3("hello there");
  flit::Variant v4(std::vector<std::string> { "a", "b", "cde", "fgh" });
  flit::Variant v5(std::vector<float> { 314159.f });
  flit::Variant v6(std::vector<double> { 3.14159e-5 });
  flit::Variant v7(std::vector<long double> { 4452346L });

  flit::Variant c1(v1);
  flit::Variant c2(v2);
  flit::Variant c3(v3);
  flit::Variant c4(v4);
  flit::Variant c5(v5);
  flit::Variant c6(v6);
  flit::Variant c7(v7);

  flit::Variant m1(std::move(c1));
  flit::Variant m2(std::move(c2));
  flit::Variant m3(std::move(c3));
  flit::Variant m4(std::move(c4));
  flit::Variant m5(std::move(c5));
  flit::Variant m6(std::move(c6));
  flit::Variant m7(std::move(c7));

  // make sure the values are the same
  TH_EQUAL(v1, m1);
  TH_EQUAL(v2, m2);
  TH_EQUAL(v3, m3);
  TH_EQUAL(v4, m4);
  TH_EQUAL(v5, m5);
  TH_EQUAL(v6, m6);
  TH_EQUAL(v7, m7);

  // make sure the moved objects are of type None
  TH_EQUAL(c1.type(), flit::Variant::Type::None);
  TH_EQUAL(c2.type(), flit::Variant::Type::None);
  TH_EQUAL(c3.type(), flit::Variant::Type::None);
  TH_EQUAL(c4.type(), flit::Variant::Type::None);
  TH_EQUAL(c5.type(), flit::Variant::Type::None);
  TH_EQUAL(c6.type(), flit::Variant::Type::None);
  TH_EQUAL(c7.type(), flit::Variant::Type::None);
}

TH_TEST(tst_Variant_val) {
  long double val2 = 3.14159L;
  std::string val3 = "hello there";
  std::vector<std::string> val4 { "a", "b", "cde", "fgh" };
  std::vector<float> val5 { 314159.f };
  std::vector<double> val6 { 3.14159e-5, 5 };
  std::vector<long double> val7 { 4452346L, 6L, 7e54L };

  flit::Variant v1;
  flit::Variant v2(val2);
  flit::Variant v3(val3);
  flit::Variant v4(val4);
  flit::Variant v5(val5);
  flit::Variant v6(val6);
  flit::Variant v7(val7);

  // cannot test that any other type will cause a linker error

  TH_THROWS(v1.val<long double>(), std::runtime_error);
  TH_THROWS(v1.val<std::string>(), std::runtime_error);
  TH_THROWS(v1.val<std::vector<std::string>>(), std::runtime_error);
  TH_THROWS(v1.val<std::vector<float>>(), std::runtime_error);
  TH_THROWS(v1.val<std::vector<double>>(), std::runtime_error);
  TH_THROWS(v1.val<std::vector<long double>>(), std::runtime_error);

  TH_EQUAL(v2.val<long double>(), val2);
  TH_THROWS(v2.val<std::string>(), std::runtime_error);
  TH_THROWS(v2.val<std::vector<std::string>>(), std::runtime_error);
  TH_THROWS(v2.val<std::vector<float>>(), std::runtime_error);
  TH_THROWS(v2.val<std::vector<double>>(), std::runtime_error);
  TH_THROWS(v2.val<std::vector<long double>>(), std::runtime_error);

  TH_THROWS(v3.val<long double>(), std::runtime_error);
  TH_EQUAL(v3.val<std::string>(), val3);
  TH_THROWS(v3.val<std::vector<std::string>>(), std::runtime_error);
  TH_THROWS(v3.val<std::vector<float>>(), std::runtime_error);
  TH_THROWS(v3.val<std::vector<double>>(), std::runtime_error);
  TH_THROWS(v3.val<std::vector<long double>>(), std::runtime_error);

  TH_THROWS(v4.val<long double>(), std::runtime_error);
  TH_THROWS(v4.val<std::string>(), std::runtime_error);
  TH_EQUAL(v4.val<std::vector<std::string>>(), val4);
  TH_THROWS(v4.val<std::vector<float>>(), std::runtime_error);
  TH_THROWS(v4.val<std::vector<double>>(), std::runtime_error);
  TH_THROWS(v4.val<std::vector<long double>>(), std::runtime_error);

  TH_THROWS(v5.val<long double>(), std::runtime_error);
  TH_THROWS(v5.val<std::string>(), std::runtime_error);
  TH_THROWS(v5.val<std::vector<std::string>>(), std::runtime_error);
  TH_EQUAL(v5.val<std::vector<float>>(), val5);
  TH_THROWS(v5.val<std::vector<double>>(), std::runtime_error);
  TH_THROWS(v5.val<std::vector<long double>>(), std::runtime_error);

  TH_THROWS(v6.val<long double>(), std::runtime_error);
  TH_THROWS(v6.val<std::string>(), std::runtime_error);
  TH_THROWS(v6.val<std::vector<std::string>>(), std::runtime_error);
  TH_THROWS(v6.val<std::vector<float>>(), std::runtime_error);
  TH_EQUAL(v6.val<std::vector<double>>(), val6);
  TH_THROWS(v6.val<std::vector<long double>>(), std::runtime_error);

  TH_THROWS(v7.val<long double>(), std::runtime_error);
  TH_THROWS(v7.val<std::string>(), std::runtime_error);
  TH_THROWS(v7.val<std::vector<std::string>>(), std::runtime_error);
  TH_THROWS(v7.val<std::vector<float>>(), std::runtime_error);
  TH_THROWS(v7.val<std::vector<double>>(), std::runtime_error);
  TH_EQUAL(v7.val<std::vector<long double>>(), val7);

  TH_EQUAL(v2.val<decltype(val2)>(), val2);
  TH_EQUAL(v3.val<decltype(val3)>(), val3);
  TH_EQUAL(v4.val<decltype(val4)>(), val4);
  TH_EQUAL(v5.val<decltype(val5)>(), val5);
  TH_EQUAL(v6.val<decltype(val6)>(), val6);
  TH_EQUAL(v7.val<decltype(val7)>(), val7);
}

TH_TEST(tst_Variant_assignmentOperator_reference) {
  flit::Variant v1;
  flit::Variant v2(3.14159);
  flit::Variant v3("hello there");
  flit::Variant v4(std::vector<std::string> { "a", "b", "cde", "fgh" });
  flit::Variant v5(std::vector<float> { 314159.f });
  flit::Variant v6(std::vector<double> { 3.14159e-5 });
  flit::Variant v7(std::vector<long double> { 4452346L });

  flit::Variant c1;
  flit::Variant c2;
  flit::Variant c3;
  flit::Variant c4;
  flit::Variant c5;
  flit::Variant c6;
  flit::Variant c7;

  TH_EQUAL(v1, c1);
  TH_NOT_EQUAL(v2, c2);
  TH_NOT_EQUAL(v3, c3);
  TH_NOT_EQUAL(v4, c4);
  TH_NOT_EQUAL(v5, c5);
  TH_NOT_EQUAL(v6, c6);
  TH_NOT_EQUAL(v7, c7);

  c1 = v1;
  c2 = v2;
  c3 = v3;
  c4 = v4;
  c5 = v5;
  c6 = v6;
  c7 = v7;

  TH_EQUAL(v1, c1);
  TH_EQUAL(v2, c2);
  TH_EQUAL(v3, c3);
  TH_EQUAL(v4, c4);
  TH_EQUAL(v5, c5);
  TH_EQUAL(v6, c6);
  TH_EQUAL(v7, c7);

  c7 = v1;
  c6 = v2;
  c5 = v3;
  c4 = v4;
  c3 = v5;
  c2 = v6;
  c1 = v7;

  TH_NOT_EQUAL(v1, c1);
  TH_NOT_EQUAL(v2, c2);
  TH_NOT_EQUAL(v3, c3);
  TH_NOT_EQUAL(v4, c3);
  TH_NOT_EQUAL(v5, c5);
  TH_NOT_EQUAL(v6, c6);
  TH_NOT_EQUAL(v7, c7);
  TH_EQUAL(v1, c7);
  TH_EQUAL(v2, c6);
  TH_EQUAL(v3, c5);
  TH_EQUAL(v4, c4);
  TH_EQUAL(v5, c3);
  TH_EQUAL(v6, c2);
  TH_EQUAL(v7, c1);
}

TH_TEST(tst_Variant_assignmentOperator_rvalueReference) {
  flit::Variant v1;
  flit::Variant v2(3.14159);
  flit::Variant v3("hello there");
  flit::Variant v4(std::vector<std::string> { "a", "b", "cde", "fgh" });
  flit::Variant v5(std::vector<float> { 314159.f });
  flit::Variant v6(std::vector<double> { 3.14159e-5 });
  flit::Variant v7(std::vector<long double> { 4452346L });

  flit::Variant c1(v1);
  flit::Variant c2(v2);
  flit::Variant c3(v3);
  flit::Variant c4(v4);
  flit::Variant c5(v5);
  flit::Variant c6(v6);
  flit::Variant c7(v7);

  TH_EQUAL(v1, c1);
  TH_EQUAL(v2, c2);
  TH_EQUAL(v3, c3);
  TH_EQUAL(v4, c4);
  TH_EQUAL(v5, c5);
  TH_EQUAL(v6, c6);
  TH_EQUAL(v7, c7);

  flit::Variant m1;
  flit::Variant m2;
  flit::Variant m3;
  flit::Variant m4;
  flit::Variant m5;
  flit::Variant m6;
  flit::Variant m7;

  TH_EQUAL(v1, m1);
  TH_NOT_EQUAL(v2, m2);
  TH_NOT_EQUAL(v3, m3);
  TH_NOT_EQUAL(v4, m4);
  TH_NOT_EQUAL(v5, m5);
  TH_NOT_EQUAL(v6, m6);
  TH_NOT_EQUAL(v7, m7);

  m1 = std::move(c1);
  m2 = std::move(c2);
  m3 = std::move(c3);
  m4 = std::move(c4);
  m5 = std::move(c5);
  m6 = std::move(c6);
  m7 = std::move(c7);

  TH_EQUAL(c1.type(), flit::Variant::Type::None);
  TH_EQUAL(c2.type(), flit::Variant::Type::None);
  TH_EQUAL(c3.type(), flit::Variant::Type::None);
  TH_EQUAL(c4.type(), flit::Variant::Type::None);
  TH_EQUAL(c5.type(), flit::Variant::Type::None);
  TH_EQUAL(c6.type(), flit::Variant::Type::None);
  TH_EQUAL(c7.type(), flit::Variant::Type::None);

  TH_EQUAL(v1, m1);
  TH_EQUAL(v2, m2);
  TH_EQUAL(v3, m3);
  TH_EQUAL(v4, m4);
  TH_EQUAL(v5, m5);
  TH_EQUAL(v6, m6);
  TH_EQUAL(v7, m7);

  c1 = v1;
  c2 = v2;
  c3 = v3;
  c4 = v4;
  c5 = v5;
  c6 = v6;
  c7 = v7;

  m7 = std::move(c1);
  m6 = std::move(c2);
  m5 = std::move(c4);
  m4 = std::move(c3);
  m3 = std::move(c5);
  m2 = std::move(c6);
  m1 = std::move(c7);

  TH_EQUAL(c1.type(), flit::Variant::Type::None);
  TH_EQUAL(c2.type(), flit::Variant::Type::None);
  TH_EQUAL(c3.type(), flit::Variant::Type::None);
  TH_EQUAL(c4.type(), flit::Variant::Type::None);
  TH_EQUAL(c5.type(), flit::Variant::Type::None);
  TH_EQUAL(c6.type(), flit::Variant::Type::None);
  TH_EQUAL(c7.type(), flit::Variant::Type::None);
  TH_NOT_EQUAL(v1, m1);
  TH_NOT_EQUAL(v2, m2);
  TH_NOT_EQUAL(v3, m3);
  TH_NOT_EQUAL(v4, m4);
  TH_NOT_EQUAL(v5, m5);
  TH_NOT_EQUAL(v6, m6);
  TH_NOT_EQUAL(v7, m7);
  TH_EQUAL(v1, m7);
  TH_EQUAL(v2, m6);
  TH_EQUAL(v3, m4);
  TH_EQUAL(v4, m5);
  TH_EQUAL(v5, m3);
  TH_EQUAL(v6, m2);
  TH_EQUAL(v7, m1);
}

TH_TEST(tst_Variant_equals) {
  flit::Variant v1;
  flit::Variant v2(3.14159);
  flit::Variant v3("hello there");
  flit::Variant v4(std::vector<std::string> { "a", "b", "cde", "fgh" });
  flit::Variant v5(std::vector<float> { 314159.f });
  flit::Variant v6(std::vector<double> { 3.14159e-5 });
  flit::Variant v7(std::vector<long double> { 4452346L });

  flit::Variant c1(v1);
  flit::Variant c2(v2);
  flit::Variant c3(v3);
  flit::Variant c4(v4);
  flit::Variant c5(v5);
  flit::Variant c6(v6);
  flit::Variant c7(v7);

  TH_VERIFY(v1.equals(c1));
  TH_VERIFY(v2.equals(c2));
  TH_VERIFY(v3.equals(c3));
  TH_VERIFY(v4.equals(c4));
  TH_VERIFY(v5.equals(c5));
  TH_VERIFY(v6.equals(c6));
  TH_VERIFY(v7.equals(c7));

  c7 = v1;
  c6 = v2;
  c5 = v3;
  c4 = v4;
  c3 = v5;
  c2 = v6;
  c1 = v7;

  TH_VERIFY(!v1.equals(c1));
  TH_VERIFY(!v2.equals(c2));
  TH_VERIFY(!v3.equals(c3));
  TH_VERIFY(!v4.equals(c3));
  TH_VERIFY(!v5.equals(c5));
  TH_VERIFY(!v6.equals(c6));
  TH_VERIFY(!v7.equals(c7));
  TH_VERIFY(v7.equals(c1));
  TH_VERIFY(v6.equals(c2));
  TH_VERIFY(v5.equals(c3));
  TH_VERIFY(v4.equals(c4));
  TH_VERIFY(v3.equals(c5));
  TH_VERIFY(v2.equals(c6));
  TH_VERIFY(v1.equals(c7));
}

TH_TEST(tst_Variant_toString) {
  flit::Variant v1;
  flit::Variant v2(3.14159);
  flit::Variant v3("hello there");
  flit::Variant v4(std::vector<std::string> { "a", "b", "cde", "fgh" });
  flit::Variant v5(std::vector<float> { 314159.f });
  flit::Variant v6(std::vector<double> { 3.14159e-5, 5 });
  flit::Variant v7(std::vector<long double> { 4452346L, 6L, 7e54L });

  TH_EQUAL(v1.toString(), "Variant(None)");
  TH_EQUAL(v2.toString(), "Variant(3.141589999999999882618340052431449294090270"
                          "99609375)");
  TH_EQUAL(v3.toString(), "Variant(string(len=11, val=\"hello there\"))");
  TH_EQUAL(v4.toString(), "Variant(vectorString{"
                          "string(len=1, val=\"a\"), "
                          "string(len=1, val=\"b\"), "
                          "string(len=3, val=\"cde\"), "
                          "string(len=3, val=\"fgh\")})");
  TH_EQUAL(v5.toString(), "Variant(vectorFloat{314159})");
  TH_EQUAL(v6.toString(), "Variant(vectorDouble{3.14158999999999985030911531236"
                          "341716066817753016948699951171875e-05, 5})");
  TH_EQUAL(v7.toString(), "Variant(vectorLongDouble{4452346, 6, 699999999999999"
                          "9999734234570895909323317109733560680448})");
}

TH_TEST(tst_Variant_toString_challenging) {
  using V = flit::Variant;
  using S = std::string;
  V v1("hi there)\"\" \";))");
  V v2(");'hello my friend ''\" \" \b\b\b");
  S s1 = "Variant(string(len=16, val=\"hi there)\"\" \";))\"))";
  S s2 = "Variant(string(len=28, val=\");'hello my friend ''\" \" \b\b\b\"))";

  TH_EQUAL(s1, v1.toString());
  TH_EQUAL(s2, v2.toString());
}

TH_TEST(tst_Variant_fromString) {
  std::string s1  = "Variant(None)";
  std::string s2  = "Variant(3.1415899999999998826183400524314492940902709960"
                    "9375)";
  std::string s3  = "Variant(string(len=11, val=\"hello there\"))";
  std::string s4  = "Variant(vectorFloat{314159})";
  std::string s5  = "Variant(vectorDouble{3.14159e-05, 5})";
  std::string s6  = "Variant(vectorLongDouble{4.45235e+06, 6, 7e+54})";
  std::string s7  = "Hello there!";
  std::string s8  = "Variant(.678)";
  std::string s9  = "Variant(vectorFloat{})";
  std::string s10 = "Variant(vectorString{"
                      "string(len=1, val=\"a\")"
                      "string(len=1, val=\"b\")"
                      "string(len=3, val=\"cde\")"
                      "string(len=3, val=\"fgh\")})";

  flit::Variant expected1;
  flit::Variant expected2(3.14159);
  flit::Variant expected3("hello there");
  flit::Variant expected4(std::vector<float> { 314159.f });
  flit::Variant expected5(std::vector<double> { 3.14159e-5, 5 });
  flit::Variant expected6(std::vector<long double> { 4.45235e6L, 6.L, 7e54L });
  flit::Variant expected7(s7);
  flit::Variant expected8(.678L);
  flit::Variant expected9(std::vector<float> {});
  flit::Variant expected10(std::vector<std::string> { "a", "b", "cde", "fgh" });

  auto actual1  = flit::Variant::fromString(s1);
  auto actual2  = flit::Variant::fromString(s2);
  auto actual3  = flit::Variant::fromString(s3);
  auto actual4  = flit::Variant::fromString(s4);
  auto actual5  = flit::Variant::fromString(s5);
  auto actual6  = flit::Variant::fromString(s6);
  auto actual7  = flit::Variant::fromString(s7);
  auto actual8  = flit::Variant::fromString(s8);
  auto actual9  = flit::Variant::fromString(s9);
  auto actual10 = flit::Variant::fromString(s10);

  TH_EQUAL(expected1,  actual1);
  TH_EQUAL(expected2,  actual2);
  TH_EQUAL(expected3,  actual3);
  TH_EQUAL(expected4,  actual4);
  TH_EQUAL(expected5,  actual5);
  TH_EQUAL(expected6,  actual6);
  TH_EQUAL(expected7,  actual7);
  TH_EQUAL(expected8,  actual8);
  TH_EQUAL(expected9,  actual9);
  TH_EQUAL(expected10, actual10);

  // A number that cannot fit in a long double
  TH_THROWS(flit::Variant::fromString("Variant(3e99999999999999999999)"),
            std::out_of_range);

  // The beginning of a number (i.e. '.'), but not having a number there
  TH_THROWS(flit::Variant::fromString("Variant(.ruiea)"),
            std::invalid_argument);

  // Starts as a string but does not have a matching end quote
  TH_THROWS(flit::Variant::fromString(
              "Variant(string(len=19, \"Non-matching quotes))"),
            std::invalid_argument);

  // Looks like "Variant(None)", but with an unexpected space in there
  TH_THROWS(flit::Variant::fromString("Variant(None )"),
            std::invalid_argument);

  // Start of a vectorString, but without the matching ending curly brace
  TH_THROWS(flit::Variant::fromString("Variant(vectorString{"
                                      "string(len=1, val=\"a\"))"),
            std::invalid_argument);

  // Start of a vectorFloat, but without the matching ending curly brace
  TH_THROWS(flit::Variant::fromString("Variant(vectorFloat{3)"),
            std::invalid_argument);

  // Start of a vectorDouble, but without the matching ending curly brace
  TH_THROWS(flit::Variant::fromString("Variant(vectorDouble{3)"),
            std::invalid_argument);

  // Start of a vectorDouble, but without the matching ending curly brace
  TH_THROWS(flit::Variant::fromString("Variant(vectorLongDouble{3)"),
            std::invalid_argument);

  // Unrecognized type within the Variant
  TH_THROWS(flit::Variant::fromString("Variant(vectorMyType{})"),
            std::invalid_argument);
}

TH_TEST(tst_Variant_fromString_challenging) {
  using V = flit::Variant;
  using S = std::string;
  S s1 = "Variant(string(len=16, val=\"hi there)\"\" \";))\"))";
  S s2 = "Variant(string(len=28, val=\");'hello my friend ''\" \" \b\b\b\"))";
  V expected1("hi there)\"\" \";))");
  V expected2(");'hello my friend ''\" \" \b\b\b");

  TH_EQUAL(expected1, V::fromString(s1));
  TH_EQUAL(expected2, V::fromString(s2));
}

TH_TEST(tst_Variant_streamOutputOperator) {
  flit::Variant v1;
  flit::Variant v2(3.14159L);
  flit::Variant v3("hello there");
  flit::Variant v4(std::vector<std::string> { "a", "b", "cde", "fgh" });
  flit::Variant v5(std::vector<float> { 314159.f });
  flit::Variant v6(std::vector<double> { 3.14159e-5, 5 });
  flit::Variant v7(std::vector<long double> { 4452346L, 6L, 7e54L });

  auto toString = [] (const flit::Variant &v) {
    std::ostringstream out;
    out << v;
    return out.str();
  };

  TH_EQUAL(toString(v1), "Variant(None)");
  TH_EQUAL(toString(v2), "Variant(3.1415899999999999999290151153630290536966640"
                         "5022144317626953125)");
  TH_EQUAL(toString(v3), "Variant(string(len=11, val=\"hello there\"))");
  TH_EQUAL(toString(v4), "Variant(vectorString{"
                         "string(len=1, val=\"a\"), "
                         "string(len=1, val=\"b\"), "
                         "string(len=3, val=\"cde\"), "
                         "string(len=3, val=\"fgh\")})");
  TH_EQUAL(toString(v5), "Variant(vectorFloat{314159})");
  TH_EQUAL(toString(v6), "Variant(vectorDouble{3.141589999999999850309115312363"
                         "41716066817753016948699951171875e-05, 5})");
  TH_EQUAL(toString(v7), "Variant(vectorLongDouble{4452346, 6, 6999999999999999"
                         "999734234570895909323317109733560680448})");
}

// already tested by constructor tests:
// - type()
// - longDouble() conversion
// - string() conversion
// - vectorFloat() conversion
// - vectorDouble() conversion
// - vectorLongDouble() conversion
// - operator==()
