#include <flit.h>

#include <typeinfo>
#include <vector>

#include <cmath>

template <typename T>
GLOBAL
void
RaUKern(const T* tiList, size_t n, double* results){
#ifdef __CUDA__
  auto idx = blockIdx.x * blockDim.x + threadIdx.x;
#else
  auto idx = 0;
#endif
  auto theta = M_PI;
  const T* ti = tiList + (idx*n);
  auto A = flit::VectorCU<T>(ti, n);
  auto orig = A;
  A = A.rotateAboutZ_3d(theta);
  A = A.rotateAboutZ_3d(-theta);
  results[idx] = A.L1Distance(orig);
}

template <typename T>
class RotateAndUnrotate: public flit::TestBase<T> {
public:
  RotateAndUnrotate(std::string id) : flit::TestBase<T>(std::move(id)) {}

  virtual size_t getInputsPerRun() override { return 3; }
  virtual std::vector<T> getDefaultInput() override {
    return flit::Vector<T>::getRandomVector(3).getData();
  }

protected:
  virtual flit::KernelFunction<T>* getKernel() override { return RaUKern; }

  virtual flit::Variant run_impl(const std::vector<T>& ti) override {
    auto theta = M_PI;
    auto A = flit::Vector<T>(ti);
    auto orig = A;
    flit::info_stream << "Rotate and Unrotate by " << theta << " radians, A is: " << A << std::endl;
    A.rotateAboutZ_3d(theta);
    flit::info_stream << "Rotated is: " << A << std::endl;
    A.rotateAboutZ_3d(-theta);
    flit::info_stream << "Unrotated is: " << A << std::endl;
    bool equal = A == orig;
    flit::info_stream << "Are they equal? " << equal << std::endl;
    auto dist = A.L1Distance(orig);
    if(!equal){
      flit::info_stream << "error in L1 distance is: " << dist << std::endl;
      flit::info_stream << "difference between: " << (A - orig) << std::endl;
    }
    flit::info_stream << "in " << id << std::endl;
    A.dumpDistanceMetrics(orig, flit::info_stream);
    return dist;
  }

protected:
  using flit::TestBase<T>::id;
};

REGISTER_TYPE(RotateAndUnrotate)
