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

// This is the main implementation, handling cmd line params and
// running the tests!

#ifndef FLIT_H
#define FLIT_H 0

#include <flit/FlitCsv.h>
#include <flit/TestBase.h>
#include <flit/flitHelpers.h>
#include <flit/fsutil.h>
#include <flit/subprocess.h>
#include <flit/FlitEventLogger.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <unistd.h> // for hostname in logfile

#include <cstring>

// Define macros to use in the output
// These can be overridden at compile time to insert compile-time information

#ifndef FLIT_HOST
#define FLIT_HOST "HOST"
#endif // FLIT_HOST

#ifndef FLIT_COMPILER
#define FLIT_COMPILER "COMPILER"
#endif // FLIT_COMPILER

#ifndef FLIT_OPTL
#define FLIT_OPTL "OPTL"
#endif // FLIT_OPTL

#ifndef FLIT_SWITCHES
#define FLIT_SWITCHES "SWITCHES"
#endif // FLIT_SWITCHES

#ifndef FLIT_NULL
#define FLIT_NULL "NULL"
#endif // FLIT_NULL

#ifndef FLIT_FILENAME
#define FLIT_FILENAME "FILENAME"
#endif // FLIT_FILENAME

namespace flit {

/** Command-line options */
struct FlitOptions {
  bool help = false;         // show usage and exit
  bool info = false;         // show compilation info and exit
  bool listTests = false;    // list available tests and exit
  bool verbose = false;      // show debug verbose messages
  std::vector<std::string> tests; // which tests to run
  std::string precision = "all";  // which precision to use
  std::string output = "";        // output file for results.  default stdout
  bool timing = true;        // should we run timing?
  int timingLoops = -1;      // < 1 means to auto-determine the timing loops
  int timingRepeats = 3;     // return best of this many timings

  bool compareMode = false;  // compare results after running the test
  std::string compareGtFile; // ground truth results to use in compareMode
  std::vector<std::string> compareFiles; // files for compareMode
  std::string compareSuffix; // suffix to add when writing back compareFiles

  bool event_logging_enabled = false;	// capture certain flit event details such as time and parameters
  std::string event_log_file_prefix = "flit_events";	// prefix of file to store event logs

  /** Give a string representation of this struct for printing purposes */
  std::string toString() const;
private:
  /** Convert a bool to a string */
  static inline std::string boolToString(bool boolean) {
    return (boolean ? "true" : "false");
  }
};

inline std::ostream& operator<<(std::ostream& out, const FlitOptions &opt) {
  return out << opt.toString();
}

template<typename A, typename B>
struct pair_hash {
  // This is from python's implementation of hashing a tuple
  size_t operator()(const std::pair<A, B> &thepair) const {
    std::hash<A> hasherA;
    std::hash<B> hasherB;
    size_t value = 0x345678;
    value = (1000003 * value) ^ hasherA(thepair.first);
    value = (1000003 * value) ^ hasherB(thepair.second);
    return value;
  }
};

/// Parse arguments
bool isFastTrack(int argCount, char const* const argList[]);
int callFastTrack(int argCount, char* argList[]);
FlitOptions parseArguments(int argCount, char const* const argList[]);

/// Returns the usage information as a string
std::string usage(std::string progName);

/// The compilation information as a string
static std::string info =
  "Compilation information:\n"
  "  Host:               \"" FLIT_HOST     "\"\n"
  "  Compiler:           \"" FLIT_COMPILER "\"\n"
  "  Optimization level: \"" FLIT_OPTL     "\"\n"
  "  Compiler flags:     \"" FLIT_SWITCHES "\"\n"
  "  Filename:           \"" FLIT_FILENAME "\"\n";

/// Parse the results file into a vector of results
std::vector<TestResult> parseResults(std::istream &in);

/// Parse the result file to get metadata from the first row
std::unordered_map<std::string, std::string> parseMetadata(std::istream &in);

/** Removes the "_idx<num>" from the name
 *
 * The name passed in is not modified, but the shortened one is returned.
 * Optionally, you can pass in an integer pointer to have it populated with the
 * value of the <num> from the end of the idx string.
 *
 * If the name does not end in "_idx<num>", then the name is returned as-is,
 * and idx (if not nullptr) is set to -1.
 *
 * @param name Name to remove "_idx<num>" from the end
 * @param idx Pointer to an integer to store <num> (optional)
 *
 * @return shortened name with "_idx<num>" removed from the end
 */
std::string removeIdxFromName(const std::string &name, int *idx = nullptr);

/// Returns the tests that need to be run for comparisons
std::vector<std::string> calculateMissingComparisons(const FlitOptions &opt);

class TestResultMap {
public:
  using key_type = std::pair<std::string, std::string>;
  using metadata_type = std::unordered_map<std::string, std::string>;

  void loadfile(const std::string &filename) {
    std::ifstream resultfile;
    flit::ifopen(resultfile, filename);
    auto parsed = parseResults(resultfile);
    this->extend(parsed, filename);
    // one metadata object
    // TODO: map of filename -> metadata
    // TODO: map of testresult -> filename (i.e., reverse of m_filemap)
    this->mapMetadata(filename);
    this->mapFilenames(parsed, filename);
    
  }

  metadata_type* metadata(const std::string &filename) {
    return &m_metadataMap.at(filename);
  }

  metadata_type* metadata(const TestResult &result) {
    std::string filename = m_testToFileMap.at(
        key_type{result.name(), result.precision()});
    return this->metadata(filename);
  }

  std::vector<TestResult*> operator[](
      const key_type &key) const
  {
    std::vector<TestResult*> all_vals;
    auto range = m_testmap.equal_range(key);
    for (auto iter = range.first; iter != range.second; iter++) {
      all_vals.push_back(iter->second);
    }
    return all_vals;
  }

  std::vector<TestResult*> fileresults(const std::string &filename) {
    std::vector<TestResult*> all_vals;
    auto range = m_filemap.equal_range(filename);
    for (auto iter = range.first; iter != range.second; iter++) {
      all_vals.push_back(&(iter->second));
    }
    return all_vals;
  }

private:
  void append(const TestResult &result, const std::string &filename) {
    auto it = m_filemap.emplace(filename, result);
    m_testmap.emplace(key_type{result.name(), result.precision()},
                      &(it->second));
  }

  void extend(const std::vector<TestResult> &results,
              const std::string &filename)
  {
    for (auto& result : results) {
      this->append(result, filename);
    }
  }

  void mapMetadata(const std::string &filename)
  {
    metadata_type md;
    
    std::ifstream fin;
    try {
      flit::ifopen(fin, filename);
    } catch (std::ios_base::failure &ex) {
      std::cerr << "Error: file does not exist: "
                << filename << std::endl;
    }
    md = parseMetadata(fin);
    
    m_metadataMap.emplace(filename,md);
  }
  
  void mapFilenames(const std::vector<TestResult> &results,
                    const std::string &filename)
  {
    for (auto& result : results) {
      m_testToFileMap.emplace(
          key_type{result.name(), result.precision()},
          filename);
    }
  }
  
private:
  std::unordered_multimap<
    std::pair<std::string, std::string>,
    TestResult*,
    pair_hash<std::string, std::string>
    > m_testmap;   // (testname, precision) -> TestResult*

  // filename -> TestResult
  std::unordered_multimap<std::string, TestResult> m_filemap;

  // (testname, precision) -> filename
  std::unordered_map<
    std::pair<std::string, std::string>,
    std::string,
    pair_hash<std::string, std::string>
    > m_testToFileMap;
  
  // filename -> metadata
  std::unordered_map<std::string, metadata_type> m_metadataMap;
};

inline void outputResults (
    const std::vector<TestResult>& results,
    std::ostream& out,
    std::string hostname = FLIT_HOST,
    std::string compiler = FLIT_COMPILER,
    std::string optimization_level = FLIT_OPTL,
    std::string switches = FLIT_SWITCHES,
    std::string executableFilename = FLIT_FILENAME)
{
  // Output the column headers
  CsvWriter writer(out);
  writer
    << "name"
    << "host"
    << "compiler"
    << "optl"
    << "switches"
    << "precision"
    << "score_hex"
    << "score"
    << "resultfile"
    << "comparison_hex"
    << "comparison"
    << "file"
    << "nanosec";
  writer.new_row();
  for (const auto& result: results) {
    writer
      << result.name()                               // test case name
      << hostname                                    // hostname
      << compiler                                    // compiler
      << optimization_level                          // optimization level
      << switches                                    // compiler flags
      << result.precision()                          // precision
      ;

    if (result.result().type() == Variant::Type::LongDouble) {
      writer
        << as_int(result.result().longDouble())      // score_hex
        << result.result().longDouble()              // score
        ;
    } else {
      writer
        << FLIT_NULL                                 // score_hex
        << FLIT_NULL                                 // score
        ;
    }

    if (result.resultfile().empty()) {
      writer << FLIT_NULL;                           // resultfile
    } else {
      writer << result.resultfile();                 // resultfile
    }

    if (result.is_comparison_null()) {
      writer
        << FLIT_NULL                                 // comparison_hex
        << FLIT_NULL                                 // comparison
        ;
    } else {
      writer
        << as_int(result.comparison())               // comparison_hex
        << result.comparison()                       // comparison
        ;
    }

    writer
      << executableFilename                          // executable filename
      << result.nanosecs()                           // nanoseconds
      ;
    writer.new_row();
  }
}


template <typename F>
void runTestWithDefaultInput(TestFactory* factory,
                             std::vector<TestResult>& totResults,
                             const std::string &filebase = "",
                             bool shouldTime = true,
                             int timingLoops = -1,
                             int timingRepeats = 3,
                             int idx = -1) {
  auto test = factory->get<F>();
  auto test_input = test->getDefaultInput();
  auto results = test->run(test_input, filebase, shouldTime, timingLoops,
                           timingRepeats, idx);
  totResults.insert(totResults.end(), results.begin(), results.end());
  info_stream.flushout();
}

template <typename F>
long double runComparison_impl(TestFactory* factory, const TestResult &gt,
                               const TestResult &res) {
  auto test = factory->get<F>();
  if (res.result().type() != gt.result().type()) {
    throw std::invalid_argument("Result and baseline comparison types do not"
                                " match");
  }
  if (!gt.resultfile().empty()) {
    if (gt.result().type() != Variant::Type::None) {
      throw std::invalid_argument("baseline comparison type is not None when"
                                  " the resultfile is defined");
    }
    Variant gtval = Variant::fromString(flit::readfile(gt.resultfile()));
    Variant resval = Variant::fromString(flit::readfile(res.resultfile()));
    return test->variant_compare(gtval, resval);
  }
  return test->variant_compare(gt.result(), res.result());
}

inline long double runComparison(TestFactory* factory, const TestResult &gt,
                                 const TestResult &res) {
  if (res.precision() == "f") {
    return runComparison_impl<float>(factory, gt, res);
  } else if (res.precision() == "d") {
    return runComparison_impl<double>(factory, gt, res);
  } else if (res.precision() == "e") {
    return runComparison_impl<long double>(factory, gt, res);
  } else { throw std::runtime_error("Unrecognized precision encountered"); }
}

/** Returns the keys of a std::map as a std::vector */
template<typename A, typename B>
std::vector<A> getKeys(std::map<A, B> map) {
  std::vector<A> keys;
  for (auto pair : map) {
    keys.emplace_back(pair.first);
  }
  return keys;
}

class ParseException : public std::exception {
public:
  ParseException(const std::string& message) : _message(message) {}
  virtual const char* what() const noexcept { return _message.c_str(); }
private:
  const std::string _message;
};

inline int runFlitTests(int argc, char* argv[]) {
  // Fast track means calling a user's main() function
  if (isFastTrack(argc, argv)) { return callFastTrack(argc, argv); }

  // Argument parsing
  // TODO: add --log-file to parser
  FlitOptions options;
  try {
    options = parseArguments(argc, argv);
  } catch (ParseException &ex) {
    std::cerr << "Error: " << ex.what() << "\n"
              << "  Use the --help option for more information\n";
    return 1;
  }

  if (options.help) {
    std::cout << usage(argv[0]);
    return 0;
  }

  if (options.info) {
    std::cout << info;
    return 0;
  }

  if (options.listTests) {
    for (auto& test : getKeys(getTests())) {
      std::cout << test << std::endl;
    }
    return 0;
  }

  if (options.verbose) {
    info_stream.show();
  }

  // setup event logging
  flit::FlitEventLogger logger; // create the logger singleton
  logger.set_program_name(argv[0]);
  std::ofstream log_out;
  // TODO: add event_logging_enabled to FlitOptions
  // TODO: add event_log_file to FlitOptions
  // TODO: log events where they happen with flit::logger->log_event()
  
  // Generate filename
  // need to make sure this PID solution is general purpose...
  pid_t pid = getpid();
  std::string logsuffix = ".log";
  // TODO: Add thread / process id?
  std::string logfile = options.event_log_file_prefix + "-cpplog-" 
                      + FLIT_HOST + "-"+ std::to_string(pid) + logsuffix;
  
  if (options.event_logging_enabled) {
    try {
      // TODO: This function in fsutil.h doesn't open to append
      //flit::ofopen(log_out, options.event_log_file_prefix);
      log_out.open(logfile, std::fstream::in | std::fstream::out | std::fstream::app);
    } catch (std::ios::failure &ex) {
      std::cerr << "Error: could not write to " << logfile << std::endl;
      return 1;
    }
    logger.set_stream(log_out);
  }

  std::unique_ptr<std::ostream> stream_deleter;
  std::ostream *outstream = &std::cout;
  std::string test_result_filebase(FLIT_FILENAME);
  if (!options.output.empty()) {
    stream_deleter.reset(new std::ofstream());
    outstream = stream_deleter.get();
    try {
      flit::ofopen(static_cast<std::ofstream&>(*outstream), options.output);
    } catch (std::ios::failure &ex) {
      std::cerr << "Error: failed to open " << options.output << std::endl;
      return 1;
    }
    test_result_filebase = options.output;
  }

  std::vector<TestResult> results;

  // if comparison mode, then find out which tests we need to run
  if (options.compareMode) {
    options.tests = calculateMissingComparisons(options);
    if (!options.compareGtFile.empty()) {
      std::ifstream fin;
      try {
        flit::ifopen(fin, options.compareGtFile);
      } catch (std::ios::failure &ex) {
        std::cerr << "Error: file does not exist: " << options.compareGtFile
                  << std::endl;
        return 1;
      }
      results = parseResults(fin);
    }
  }

  std::cout.precision(1000); //set cout to print many decimal places
  info_stream.precision(1000);

  auto testMap = getTests();
  for (auto& testName : options.tests) {
    int idx;
    auto factory = testMap[removeIdxFromName(testName, &idx)];
    if (options.precision == "all" || options.precision == "float") {
      runTestWithDefaultInput<float>(factory, results, test_result_filebase,
                                     options.timing, options.timingLoops,
                                     options.timingRepeats, idx);
    }
    if (options.precision == "all" || options.precision == "double") {
      runTestWithDefaultInput<double>(factory, results, test_result_filebase,
                                      options.timing, options.timingLoops,
                                      options.timingRepeats, idx);
    }
    if (options.precision == "all" || options.precision == "long double") {
      runTestWithDefaultInput<long double>(
          factory, results, test_result_filebase, options.timing,
          options.timingLoops, options.timingRepeats, idx);
    }
  }

  // Sort the results first by name then by precision
  auto testComparator = [](const TestResult &a, const TestResult &b) {
    if (a.name() != b.name()) {
      return a.name() < b.name();
    } else {
      return a.precision() < b.precision();
    }
  };
  std::sort(results.begin(), results.end(), testComparator);

  // Let's now run the ground-truth comparisons
  if (options.compareMode) {
    TestResultMap comparisonResults;

    for (auto fname : options.compareFiles) {
      try {
        comparisonResults.loadfile(fname);
      } catch (std::ios::failure &ex) {
        std::cerr << "Error: failed to open file " << fname << std::endl;
        return 1;
      }
    }

    // compare mode is only done in the ground truth compilation
    // so "results" are the ground truth results.
    for (auto& gtres : results) {
      auto factory = testMap[removeIdxFromName(gtres.name())];
      auto toCompare = comparisonResults[{gtres.name(), gtres.precision()}];

      for (TestResult* compResult : toCompare) {
        auto &metadata = *comparisonResults.metadata(*compResult);
        // Error: std::invalid_argument "No column named 'host'"
	      //   in parseMetadata
	      //      
	      //std::unordered_map<std::string, std::string> metadata;
        //{
        //  std::ifstream fin;
        //  try {
        //    flit::ifopen(fin, compResult->resultfile());
        //  } catch (std::ios_base::failure &ex) {
        //    std::cerr << "Error: file does not exist: "
        //              << compResult->resultfile() << std::endl;
        //    return 1;
        //  }
        //  metadata = parseMetadata(fin);
        //}
        flit::logger->log_event("TestResultCompare", flit::FlitEventLogger::START,
                                {{"test-name", gtres.name()},
                                 {"other-precision", gtres.precision()},
                                 {"other-host"     , metadata["host"]},
                                 {"other-compiler" , metadata["compiler"]},
                                 {"other-optl"     , metadata["optl"]},
                                 {"other-switches" , metadata["switches"]},
                                 {"executable-name" , metadata["file"]},
                                });
        auto compVal = runComparison(factory, gtres, *compResult);
        flit::logger->log_event("TestResultCompare", flit::FlitEventLogger::STOP,
                                {{"test-name", gtres.name()},
                                 {"other-precision", gtres.precision()},
                                 {"other-host"     , metadata["host"]},
                                 {"other-compiler" , metadata["compiler"]},
                                 {"other-optl"     , metadata["optl"]},
                                 {"other-switches" , metadata["switches"]},
                                 {"executable-name" , metadata["file"]},
                                });
        compResult->set_comparison(compVal);
      }
    }

    // save back to the compare files with compare value set
    for (auto fname : options.compareFiles) {
      auto &metadata = *comparisonResults.metadata(fname);
      // read in the metadata to use in creating the file again
      //std::unordered_map<std::string, std::string> metadata;
      //{
      //  std::ifstream fin;
      //  try {
      //    flit::ifopen(fin, fname);
      //  } catch (std::ios_base::failure &ex) {
      //    std::cerr << "Error: file does not exist: " << fname << std::endl;
      //    return 1;
      //  }
      //  metadata = parseMetadata(fin);
      //}

      // get all results from this file
      auto fileresultPtrs = comparisonResults.fileresults(fname);
      std::vector<TestResult> fileresults;
      for (auto resultPtr : fileresultPtrs) {
        fileresults.push_back(*resultPtr);
      }

      // sort the file results
      std::sort(fileresults.begin(), fileresults.end(), testComparator);

      // output back to a file
      {
        std::ofstream fout;
        try {
          flit::ofopen(fout, fname + options.compareSuffix);
        } catch (std::ios::failure &ex) {
          std::cerr << "Error: could not write to " << fname << std::endl;
          return 1;
        }
        outputResults(
            fileresults,
            fout,
            metadata["host"],
            metadata["compiler"],
            metadata["optl"],
            metadata["switches"],
            metadata["file"]
            );
      }
    }
  }

  // Create the main results output
  outputResults(results, *outstream);

  return 0;
}

} // end of namespace flit

#endif // FLIT_H
