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
 * -- LICENSE END -- */

#include "Matrix.h"
#include "Vector.h"

#include <flit/flit.h>

#include <typeinfo>

#include <cmath>

template <typename T>
class DoSkewSymCPRotationTest: public flit::TestBase<T> {
public:
  DoSkewSymCPRotationTest(std::string id)
    : flit::TestBase<T>(std::move(id)) {}

  virtual size_t getInputsPerRun() override { return 6; }
  virtual std::vector<T> getDefaultInput() override {
    auto n = getInputsPerRun();
    return Vector<T>::getRandomVector(n).getData();
  }

protected:
  virtual flit::Variant run_impl(const std::vector<T>& ti) override {
    flit::info_stream << "entered " << id << std::endl;
    long double L1Score = 0.0;
    Vector<T> A = { ti[0], ti[1], ti[2] };
    Vector<T> B = { ti[3], ti[4], ti[5] };
    A = A.getUnitVector();
    B = B.getUnitVector();
    flit::info_stream << "A (unit) is: " << std::endl << A << std::endl;
    flit::info_stream << "B (unit): " << std::endl  << B << std::endl;
    auto cross = A.cross(B); //cross product
    flit::info_stream << "cross: " << std::endl << cross << std::endl;
    auto sine = cross.L2Norm();
    flit::info_stream << "sine: " << std::endl << sine << std::endl;
    //    crit = A ^ B; //dot product
    auto cos = A ^ B;
    //    flit::info_stream << "cosine: " << std::endl << crit << std::endl;
    flit::info_stream << "cosine: " << std::endl << cos << std::endl;
    auto sscpm = Matrix<T>::SkewSymCrossProdM(cross);
    flit::info_stream << "sscpm: " << std::endl << sscpm << std::endl;
    auto rMatrix = Matrix<T>::Identity(3) +
      sscpm + (sscpm * sscpm) * ((1 - cos)/(sine * sine));
    // auto rMatrix = Matrix<T>::Identity(3) +
    //   sscpm + (sscpm * sscpm) * ((1 - crit)/(sine * sine));
    auto result = rMatrix * A;
    flit::info_stream << "rotator: " << std::endl << rMatrix << std::endl;
    if(!(result == B)){
      L1Score = result.L1Distance(B);
      flit::info_stream << "Skew symmetric cross product rotation failed with ";
      flit::info_stream << "L1Distance " << L1Score << std::endl;
      flit::info_stream << "starting vectors: " << std::endl;
      flit::info_stream << A << std::endl;
      flit::info_stream << "...and..." << std::endl;
      flit::info_stream << B << std::endl;
      flit::info_stream << "ended up with: " << std::endl;
      flit::info_stream << "L1Distance: " << L1Score << std::endl;
    }
    return L1Score;
  }

private:
  using flit::TestBase<T>::id;
};

REGISTER_TYPE(DoSkewSymCPRotationTest)
