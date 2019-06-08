/* -- LICENSE BEGIN --
 *
 * Copyright (c) 2015-2019, Lawrence Livermore National Security, LLC.
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
#include "fsutil.h"

#define time_function wrap_time_function
#define time_function_autoloop wrap_time_function_autoloop
#include "TestBase.h"
#undef time_function
#undef time_function_autoloop

#include "MpiEnvironment.h"
 
#include <sstream>

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

int time_function_invocations = 0;
int time_function_autoloop_invocations = 0;
size_t last_loops = 0;
size_t last_repeats = 0;

} // end of unnamed namespace

// define stubs
namespace flit {

int_fast64_t time_function(
    const TimingFunction &func, size_t loops = 1, size_t repeats = 3);
int_fast64_t time_function_autoloop(const TimingFunction &func,
                                    size_t repeats = 3);

int_fast64_t wrap_time_function(
    const TimingFunction &func, size_t loops, size_t repeats)
{
  time_function_invocations++;
  last_loops = loops;
  last_repeats = repeats;
  // Only call the test once
  return time_function(func, 1, 1);
}

int_fast64_t wrap_time_function_autoloop(
    const TimingFunction &func, size_t repeats)
{
  time_function_autoloop_invocations++;
  last_repeats = repeats;
  return time_function(func, 1 ,1);
}

} // end of namespace flit


namespace TestResultTests {

void tst_TestResult_constructor() {
  std::string name = "hello";
  std::string precision = "double";
  flit::Variant result("string result");
  int_fast64_t nanosecs = 1900;
  std::string resultfile = "filename-here";

  flit::TestResult r(name, precision, result, nanosecs, resultfile);

  TH_EQUAL(r.name(), name);
  TH_EQUAL(r.precision(), precision);
  TH_EQUAL(r.result(), result);
  TH_EQUAL(r.nanosecs(), nanosecs);
  TH_EQUAL(r.comparison(), 0.0L);
  TH_EQUAL(r.is_comparison_null(), true);
  TH_EQUAL(r.resultfile(), resultfile);
}
TH_REGISTER(tst_TestResult_constructor);

void tst_TestResult_setters() {
  std::string name = "hello";
  std::string precision = "double";
  flit::Variant result("string result");
  int_fast64_t nanosecs = 1900;
  std::string resultfile = "filename-here";
  std::string resultfile2 = "now-a-new-filename";

  flit::TestResult r(name, precision, result, nanosecs, resultfile);

  r.set_comparison(3.14L);
  TH_EQUAL(r.comparison(), 3.14L);
  TH_VERIFY(!r.is_comparison_null());

  r.set_resultfile(resultfile2);
  TH_EQUAL(r.resultfile(), resultfile2);
}
TH_REGISTER(tst_TestResult_setters);

void tst_TestResult_stream() {
  using flit::operator<<;
  std::ostringstream out;

  std::string name = "hello";
  std::string precision = "double";
  flit::Variant result("string result");
  int_fast64_t nanosecs = 1900;
  std::string resultfile = "filename-here";
  std::string resultfile2 = "now-a-new-filename";

  flit::TestResult r(name, precision, result, nanosecs, resultfile);

  out << r;
  std::string expected(
      "hello:double,Variant(\"string result\"),0.000000,1900");
  std::string actual(out.str());

  TH_EQUAL(actual, expected);
}
TH_REGISTER(tst_TestResult_stream);

}

namespace TestBase {

template <typename F>
class MocTest : public flit::TestBase<F> {
public:
  using flit::TestBase<F>::TestBase;
  virtual size_t getInputsPerRun() { return inputsPerRun; }
  virtual std::vector<F> getDefaultInput() { return defaultInputs; }

  virtual long double compare(long double ground_truth,
                              long double test_results) const {
    compareT(ground_truth, test_results);
    return 1;
  }

  /** There is no good default implementation comparing two strings */
  virtual long double compare(const std::string &ground_truth,
                              const std::string &test_results) const {
    compareT(ground_truth, test_results);
    return 2;
  }

  virtual long double compare(const std::vector<F> &ground_truth,
                              const std::vector<F> &test_results) const {
    compareT(ground_truth, test_results);
    return 3;
  }

  virtual flit::Variant run_impl(const std::vector<F>& ti) {
    TH_EQUAL(ti.size(), getInputsPerRun());
    inputs.push_back(ti);
    return to_return;
  }

protected:
  template <typename T>
  void compareT(const T &ground_truth, const T &test_results) const {
    TH_EQUAL(gt_compare.type(), flit::Variant::Type::None);
    TH_EQUAL(tr_compare.type(), flit::Variant::Type::None);
    gt_compare = ground_truth;
    tr_compare = test_results;
  }

public:
  mutable flit::Variant gt_compare; // ground truth value used in compare()
  mutable flit::Variant tr_compare; // test result value used in compare()
  flit::Variant to_return;  // return value from run_impl
  std::vector<std::vector<F>> inputs;
  using flit::TestBase<F>::id;
  int inputsPerRun = 2;
  std::vector<F> defaultInputs { 3, 4 };
};

void tst_TestBase_constructor() {
  std::string id("This is my ID");
  MocTest<float> test(id);
  TH_EQUAL(test.id, id);
}
TH_REGISTER(tst_TestBase_constructor);

void tst_TestBase_variant_compare() {
  std::string id("This is my ID");
  MocTest<float> t1(id);
  MocTest<float> t2(id);
  MocTest<float> t3(id);
  MocTest<double> t4(id);
  MocTest<long double> t5(id);
  std::vector<flit::Variant> gts {
    3.14L,
    "Bentley",
    std::vector<float>{3.14f, 3.f},
    std::vector<double>{3.14},
    std::vector<long double>{},
  };

  // different types
  TH_THROWS(t3.variant_compare(gts[0], gts[1]), std::runtime_error);

  // test<T> not matching vector<F>
  TH_THROWS(t3.variant_compare(gts[3], gts[3]), std::runtime_error);
  TH_THROWS(t3.variant_compare(gts[4], gts[4]), std::runtime_error);
  TH_THROWS(t4.variant_compare(gts[2], gts[2]), std::runtime_error);
  TH_THROWS(t4.variant_compare(gts[4], gts[4]), std::runtime_error);
  TH_THROWS(t5.variant_compare(gts[2], gts[2]), std::runtime_error);
  TH_THROWS(t5.variant_compare(gts[3], gts[3]), std::runtime_error);

  TH_EQUAL(t1.variant_compare(gts[0], gts[0]), 1.0L);
  TH_EQUAL(t2.variant_compare(gts[1], gts[1]), 2.0L);
  TH_EQUAL(t3.variant_compare(gts[2], gts[2]), 3.0L);
  TH_EQUAL(t4.variant_compare(gts[3], gts[3]), 3.0L);
  TH_EQUAL(t5.variant_compare(gts[4], gts[4]), 3.0L);
}
TH_REGISTER(tst_TestBase_variant_compare);

template <typename F>
struct RunSetup {
  MocTest<F> test;
  flit::MpiEnvironment mpi;
};

template <typename F>
RunSetup<F> run_setup() {
  int argc = 1;
  char name[] = "run()";
  char * argv[1] { name };
  char ** argvref = argv;
  flit::MpiEnvironment mpi_env(argc, argvref);
  std::string id("This-is-my-ID");
  MocTest<F> test(id);
  RunSetup<F> setup {MocTest<F>(id), mpi_env};
  flit::mpi = &setup.mpi;
  return setup;
}

void tst_TestBase_run_noInputs() {
  auto setup = run_setup<float>();
  auto &test = setup.test;
  test.to_return = 3;

  auto results = test.run({}, "", false);
  TH_EQUAL(test.inputs.size(), 0); // verify run_impl was not called
  TH_EQUAL(results.size(), 0);

  results = test.run({3}, "", false);  // less than one run
  TH_EQUAL(test.inputs.size(), 0); // verify run_impl was not called
  TH_EQUAL(results.size(), 0);
}
TH_REGISTER(tst_TestBase_run_noInputs);

void tst_TestBase_run_oneInput() {
  auto setup = run_setup<float>();
  auto &test = setup.test;
  test.to_return = 3;

  std::vector<float> ti {2, 3};
  auto results = test.run(ti, "", false);

  TH_EQUAL(test.inputs.size(), 1); // verify run_impl was called once
  TH_EQUAL(test.inputs[0], ti);
  TH_EQUAL(results.size(), 1);
  TH_EQUAL(results[0].name(), test.id);
  TH_EQUAL(results[0].precision(), "f");
  TH_EQUAL(results[0].result(), 3);
  TH_EQUAL(results[0].nanosecs(), 0);
  TH_EQUAL(results[0].resultfile(), "");
  test.inputs.clear();

  ti = {5, 1, 4};
  std::vector<float> expected_input {5, 1};
  results = test.run(ti, "", false);
  TH_EQUAL(test.inputs.size(), 1); // verify run_impl was called once
  TH_EQUAL(test.inputs[0], expected_input);
}
TH_REGISTER(tst_TestBase_run_oneInput);

void tst_TestBase_run_twoInputs() {
  auto setup = run_setup<double>();
  auto &test = setup.test;
  test.to_return = 3.14L;

  auto results = test.run({3, 4, 5, 6}, "", false);
  std::vector<double> expected_1 = {3, 4};
  std::vector<double> expected_2 = {5, 6};
  TH_EQUAL(test.inputs.size(), 2); // verify run_impl was called once
  TH_EQUAL(test.inputs[0], expected_1);
  TH_EQUAL(test.inputs[1], expected_2);
  TH_EQUAL(results.size(), 2);
  TH_EQUAL(results[0].name(), test.id + "_idx0");
  TH_EQUAL(results[0].precision(), "d");
  TH_EQUAL(results[0].result(), 3.14L);
  TH_EQUAL(results[0].nanosecs(), 0);
  TH_EQUAL(results[0].resultfile(), "");
  TH_EQUAL(results[1].name(), test.id + "_idx1");
  TH_EQUAL(results[1].precision(), "d");
  TH_EQUAL(results[1].result(), 3.14L);
  TH_EQUAL(results[1].nanosecs(), 0);
  TH_EQUAL(results[1].resultfile(), "");
  test.inputs.clear();

  results = test.run({7, 6, 5, 4, 3}, "", false);
  expected_1 = {7, 6};
  expected_2 = {5, 4};
  TH_EQUAL(test.inputs.size(), 2); // verify run_impl was called once
  TH_EQUAL(test.inputs[0], expected_1);
  TH_EQUAL(test.inputs[1], expected_2);
  TH_EQUAL(results.size(), 2);
  TH_EQUAL(results[0].name(), test.id + "_idx0");
  TH_EQUAL(results[0].precision(), "d");
  TH_EQUAL(results[0].result(), 3.14L);
  TH_EQUAL(results[0].nanosecs(), 0);
  TH_EQUAL(results[0].resultfile(), "");
  TH_EQUAL(results[1].name(), test.id + "_idx1");
  TH_EQUAL(results[1].precision(), "d");
  TH_EQUAL(results[1].result(), 3.14L);
  TH_EQUAL(results[1].nanosecs(), 0);
  TH_EQUAL(results[1].resultfile(), "");
  test.inputs.clear();
}
TH_REGISTER(tst_TestBase_run_twoInputs);

void tst_TestBase_run_manyInputs() {
  auto setup = run_setup<long double>();
  auto &test = setup.test;
  test.to_return = 5e10L;

  std::vector<long double> ti = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
                                 1, 2, 3, 4, 5, 6, 7,  8,  9, 10,
                                 0, -1};
  auto results = test.run(ti, "", false);
  std::vector<long double> expected_1  = { 3,  4};
  std::vector<long double> expected_2  = { 5,  6};
  std::vector<long double> expected_3  = { 7,  8};
  std::vector<long double> expected_4  = { 9, 10};
  std::vector<long double> expected_5  = {11, 12};
  std::vector<long double> expected_6  = { 1,  2};
  std::vector<long double> expected_7  = { 3,  4};
  std::vector<long double> expected_8  = { 5,  6};
  std::vector<long double> expected_9  = { 7,  8};
  std::vector<long double> expected_10 = { 9, 10};
  std::vector<long double> expected_11 = { 0, -1};
  TH_EQUAL(test.inputs.size(), 11); // verify run_impl was called 11 times
  TH_EQUAL(test.inputs[ 0], expected_1);
  TH_EQUAL(test.inputs[ 1], expected_2);
  TH_EQUAL(test.inputs[ 2], expected_3);
  TH_EQUAL(test.inputs[ 3], expected_4);
  TH_EQUAL(test.inputs[ 4], expected_5);
  TH_EQUAL(test.inputs[ 5], expected_6);
  TH_EQUAL(test.inputs[ 6], expected_7);
  TH_EQUAL(test.inputs[ 7], expected_8);
  TH_EQUAL(test.inputs[ 8], expected_9);
  TH_EQUAL(test.inputs[ 9], expected_10);
  TH_EQUAL(test.inputs[10], expected_11);
  TH_EQUAL(results.size(), 11);
  for (auto &result : results) {
    TH_EQUAL(result.precision(), "e");
    TH_EQUAL(result.result(), 5e10L);
    TH_EQUAL(result.nanosecs(), 0);
    TH_EQUAL(result.resultfile(), "");
  }
  TH_EQUAL(results[ 0].name(), test.id + "_idx0");
  TH_EQUAL(results[ 1].name(), test.id + "_idx1");
  TH_EQUAL(results[ 2].name(), test.id + "_idx2");
  TH_EQUAL(results[ 3].name(), test.id + "_idx3");
  TH_EQUAL(results[ 4].name(), test.id + "_idx4");
  TH_EQUAL(results[ 5].name(), test.id + "_idx5");
  TH_EQUAL(results[ 6].name(), test.id + "_idx6");
  TH_EQUAL(results[ 7].name(), test.id + "_idx7");
  TH_EQUAL(results[ 8].name(), test.id + "_idx8");
  TH_EQUAL(results[ 9].name(), test.id + "_idx9");
  TH_EQUAL(results[10].name(), test.id + "_idx10");
  test.inputs.clear();
}
TH_REGISTER(tst_TestBase_run_manyInputs);

void tst_TestBase_run_idxWithOneInput() {
  auto setup = run_setup<float>();
  auto &test = setup.test;
  test.to_return = 3;

  std::vector<float> ti {2, 3};
  auto results = test.run(ti, "", false, 5, 5, 0);

  TH_EQUAL(test.inputs.size(), 1); // verify run_impl was called once
  TH_EQUAL(test.inputs[0], ti);
  TH_EQUAL(results.size(), 1);
  TH_EQUAL(results[0].name(), test.id);
  TH_EQUAL(results[0].precision(), "f");
  TH_EQUAL(results[0].result(), 3);
  TH_EQUAL(results[0].nanosecs(), 0);
  TH_EQUAL(results[0].resultfile(), "");
  test.inputs.clear();

  ti = {5, 1, 4};
  std::vector<float> expected_input {5, 1};
  results = test.run(ti, "", false);
  TH_EQUAL(test.inputs.size(), 1); // verify run_impl was called once
  TH_EQUAL(test.inputs[0], expected_input);
}
TH_REGISTER(tst_TestBase_run_idxWithOneInput);

void tst_TestBase_run_idxWithManyInputs() {
  auto setup = run_setup<long double>();
  auto &test = setup.test;
  test.to_return = 5e10L;

  std::vector<long double> ti = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
                                 1, 2, 3, 4, 5, 6, 7,  8,  9, 10,
                                 0, -1};
  auto results = test.run(ti, "", false, 5, 5, 6);
  std::vector<long double> expected_7  = { 3,  4};
  TH_EQUAL(test.inputs.size(), 1); // verify run_impl was called 1 time
  TH_EQUAL(test.inputs[ 0], expected_7);
  TH_EQUAL(results.size(), 1);
  for (auto &result : results) {
    TH_EQUAL(result.precision(), "e");
    TH_EQUAL(result.result(), 5e10L);
    TH_EQUAL(result.nanosecs(), 0);
    TH_EQUAL(result.resultfile(), "");
  }
  TH_EQUAL(results[ 0].name(), test.id + "_idx6");
  test.inputs.clear();
}
TH_REGISTER(tst_TestBase_run_idxWithManyInputs);

void tst_TestBase_run_disabledTests() {
  auto setup = run_setup<long double>();
  auto &test = setup.test;
  test.to_return = flit::Variant(); // None type

  std::vector<long double> ti {5, 4, 3, 2, 1};
  std::vector<long double> expected_1 {5, 4};
  std::vector<long double> expected_2 {3, 2};
  auto results = test.run(ti, "");
  TH_EQUAL(test.inputs.size(), 2); // verify run_impl was called 1 time
  TH_EQUAL(test.inputs[ 0], expected_1);
  TH_EQUAL(test.inputs[ 1], expected_2);
  TH_EQUAL(results.size(), 0);
  test.inputs.clear();
}
TH_REGISTER(tst_TestBase_run_disabledTests);

void tst_TestBase_run_shouldNotTime() {
  auto setup = run_setup<long double>();
  auto &test = setup.test;
  test.to_return = 5e10L;
  time_function_invocations = 0;
  time_function_autoloop_invocations = 0;

  std::vector<long double> ti {5, 4};
  std::vector<long double> expected_1 {5, 4};
  auto results = test.run(ti, "", false, 1000, 1000);
  TH_EQUAL(test.inputs.size(), 1); // verify run_impl was called 1 time
  TH_EQUAL(test.inputs[ 0], expected_1);
  TH_EQUAL(results.size(), 1);
  test.inputs.clear();

  TH_EQUAL(time_function_invocations, 0);
  TH_EQUAL(time_function_autoloop_invocations, 0);
}
TH_REGISTER(tst_TestBase_run_shouldNotTime);

void tst_TestBase_run_autoLooping() {
  auto setup = run_setup<long double>();
  auto &test = setup.test;
  test.to_return = 5e10L;
  time_function_invocations = 0;
  time_function_autoloop_invocations = 0;
  last_repeats = 0;

  std::vector<long double> ti {5, 4};
  std::vector<long double> expected_1 {5, 4};
  size_t expected_repeats = 50;
  auto results = test.run(ti, "", true, -1, expected_repeats);
  TH_EQUAL(test.inputs.size(), 1); // verify run_impl was called 1 time
  TH_EQUAL(test.inputs[ 0], expected_1);
  TH_EQUAL(results.size(), 1);
  test.inputs.clear();

  // test that time_function_autoloop() is called and not time_function()
  TH_EQUAL(time_function_invocations, 0);
  TH_EQUAL(time_function_autoloop_invocations, 1);
  TH_EQUAL(last_repeats, expected_repeats);
}
TH_REGISTER(tst_TestBase_run_autoLooping);

void tst_TestBase_run_specifiedLoops() {
  auto setup = run_setup<long double>();
  auto &test = setup.test;
  test.to_return = 5e10L;
  time_function_invocations = 0;
  time_function_autoloop_invocations = 0;
  last_loops = 0;
  last_repeats = 0;

  std::vector<long double> ti {5, 4};
  std::vector<long double> expected_1 {5, 4};
  size_t expected_loops = 125;
  size_t expected_repeats = 50;
  auto results = test.run(ti, "", true, expected_loops, expected_repeats);
  TH_EQUAL(test.inputs.size(), 1); // verify run_impl was called 1 time
  TH_EQUAL(test.inputs[ 0], expected_1);
  TH_EQUAL(results.size(), 1);
  test.inputs.clear();

  // test that time_function() is called and not time_function_autoloop()
  TH_EQUAL(time_function_invocations, 1);
  TH_EQUAL(time_function_autoloop_invocations, 0);
  TH_EQUAL(last_loops, expected_loops);
  TH_EQUAL(last_repeats, expected_repeats);
}
TH_REGISTER(tst_TestBase_run_specifiedLoops);

void tst_TestBase_run_outputFileUsingFilebase() {
  // test the output file using the filebase
  auto setup = run_setup<double>();
  auto &test = setup.test;
  test.inputsPerRun = 0;
  test.to_return = std::vector<double> { 1, 2, 3 };

  std::vector<double> ti {};
  fsutil::TempDir tmpdir;
  auto filebase = tmpdir.name() + "/my-file-base";
  bool shouldTime = false;
  auto results = test.run(ti, filebase, shouldTime);
  TH_EQUAL(results.size(), 1);
  TH_EQUAL(results[0].result(), flit::Variant());

  auto dir_contents = fsutil::listdir(tmpdir.name());
  TH_VERIFY(std::any_of(dir_contents.begin(), dir_contents.end(),
            [] (std::string fname) {
              return fname == "my-file-base_This-is-my-ID_d.dat";
            }));
}
TH_REGISTER(tst_TestBase_run_outputFileUsingFilebase);

void tst_TestBase_run_outputVariantsToFile() {
  TH_SKIP("unimplemented");
  // TODO: test each variant type that they each go to file except long double
  // TODO- perhaps do it using a foreach loop with the enum:
  // TODO-   for (auto vtype : std::range(flit::Variant::Type::None,
  // TODO-                                flit::Variant::Type::Error)) { ... }
}
TH_REGISTER(tst_TestBase_run_outputVariantsToFile);

void tst_TestBase_run_variantValueInFile() {
  TH_SKIP("unimplemented");
  // TODO: test the values of the variants that go to file
}
TH_REGISTER(tst_TestBase_run_variantValueInFile);

}