/* -- LICENSE BEGIN --
 *
 * Copyright (c) 2015-2018, Lawrence Livermore National Security, LLC.
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

#ifndef SUBPROCESS_H
#define SUBPROCESS_H

#define FLIT_REGISTER_MAIN(mainfunc)                           \
  struct Flit_##mainfunc##Factory {                            \
    Flit_##mainfunc##Factory() {                               \
      flit::register_main_func(#mainfunc, mainfunc);           \
    }                                                          \
  };                                                           \
  static Flit_##mainfunc##Factory global_##mainfunc##Factory;  \

#include <string>
 
namespace flit {

struct ProcResult {
  int ret;
  std::string out;
  std::string err;
};

/** Calls a command in the shell and returns the output and result */
ProcResult call_with_output(const std::string &command);

std::ostream& operator<<(std::ostream& out, const ProcResult &res);

using MainFunc = int(int, char**);

/** Don't call this directly.  Use FLIT_REGISTER_MAIN() instead */
void register_main_func(const std::string &main_name, MainFunc *main_func);

/** Return the registered main function from the name */
MainFunc* find_main_func(const std::string &main_name);

/** Return the registered main name from the function pointer */
std::string find_main_name(MainFunc *main_func);

/** Call the main function in a child process
 *
 * It is required that the user has not changed to a different directory
 * from where it was originally called.  That way we can find and run this
 * executable.
 *
 * Example:
 *
 *   #define main mymain
 *   #include "main.cc"
 *   #undef main
 *
 *   #include <flit/flit.h>
 *
 *   #include <iostream>
 *
 *   FLIT_REGISTER_MAIN(mymain)
 *
 *   int run_my_main() {
 *     auto results = flit::call_main(
 *         mymain, "myapp", "--data '../My Documents/tmp.dat' --all");
 *     std::cout << results.out;
 *     std::cerr << results.err;
 *     return results.ret;
 *   }
 *
 * @param func: function pointer to a custom renamed main() function
 * @param progname: the name you want that main to see as the program name
 *        (i.e., it will be in argv[0])
 * @param remaining_args: all command-line arguments as would be given in a
 *        shell.
 * @return result, including stdout, stderr, and return code.
 */
ProcResult call_main(MainFunc *func, std::string progname = "",
                     std::string remaining_args = "");

ProcResult call_mpi_main(MainFunc *func, std::string mpirun_command = "mpirun",
                         std::string progname = "",
                         std::string remaining_args = "");
} // end of namespace flit

#endif // SUBPROCESS_H
