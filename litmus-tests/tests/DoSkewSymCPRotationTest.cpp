#include <flit.h>

#include <typeinfo>

#include <cmath>

template <typename T>
GLOBAL
void
DoSkewSCPRKernel(const flit::CuTestInput<T>* tiList, double* results){
#ifdef __CUDA__
  auto idx = blockIdx.x * blockDim.x + threadIdx.x;
#else
  auto idx = 0;
#endif
  const T* vals = tiList[idx].vals;
  auto A = flit::VectorCU<T>(vals, 3).getUnitVector();
  auto B = flit::VectorCU<T>(vals + 3, 3).getUnitVector();
  auto cross = A.cross(B);
  auto sine = cross.L2Norm();
  auto cos = A ^ B;
  auto sscpm = flit::MatrixCU<T>::SkewSymCrossProdM(cross);
  auto rMatrix = flit::MatrixCU<T>::Identity(3) +
    sscpm + (sscpm * sscpm) * ((1 - cos)/(sine * sine));
  auto result = rMatrix * A;
  results[idx] = result.L1Distance(B);
}

template <typename T>
class DoSkewSymCPRotationTest: public flit::TestBase<T> {
public:
  DoSkewSymCPRotationTest(std::string id)
    : flit::TestBase<T>(std::move(id)) {}

  virtual size_t getInputsPerRun() override { return 6; }
  virtual flit::TestInput<T> getDefaultInput() override {
    flit::TestInput<T> ti;
    ti.min = -6;
    ti.max = 6;
    auto n = getInputsPerRun();
    ti.highestDim = n;
    ti.vals = flit::Vector<T>::getRandomVector(n).getData();
    return ti;
  }

protected:
  virtual flit::KernelFunction<T>* getKernel() override { return DoSkewSCPRKernel;}

  virtual flit::Variant run_impl(const flit::TestInput<T>& ti) override {
    flit::info_stream << "entered " << id << std::endl;
    long double L1Score = 0.0;
    flit::Vector<T> A = { ti.vals[0], ti.vals[1], ti.vals[2] };
    flit::Vector<T> B = { ti.vals[3], ti.vals[4], ti.vals[5] };
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
    auto sscpm = flit::Matrix<T>::SkewSymCrossProdM(cross);
    flit::info_stream << "sscpm: " << std::endl << sscpm << std::endl;
    auto rMatrix = flit::Matrix<T>::Identity(3) +
      sscpm + (sscpm * sscpm) * ((1 - cos)/(sine * sine));
    // auto rMatrix = flit::Matrix<T>::Identity(3) +
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
