#include "testBase.hpp"
#include "QFPHelpers.hpp"

#include <cmath>
#include <typeinfo>

template <typename T>
class DoSimpleRotate90: public QFPTest::TestBase<T> {
public:
  DoSimpleRotate90(std::string id):QFPTest::TestBase<T>(std::move(id)) {}

  QFPTest::ResultType operator()(const QFPTest::TestInput& ti) {
    Q_UNUSED(ti);

    QFPHelpers::Vector<T> A = {1, 1, 1};
    QFPHelpers::Vector<T> expected = {-1, 1, 1};
    QFPHelpers::info_stream << "Rotating A: " << A << ", 1/2 PI radians" << std::endl;
    A.rotateAboutZ_3d(M_PI/2);
    QFPHelpers::info_stream << "Resulting vector: " << A << std::endl;
    QFPHelpers::info_stream << "in " << id << std::endl;
    A.dumpDistanceMetrics(expected, QFPHelpers::info_stream);
    return {{
      {id, typeid(T).name()}, {A.L1Distance(expected), A.LInfDistance(expected)}
    }};
  }

private:
  using QFPTest::TestBase<T>::id;
};

REGISTER_TYPE(DoSimpleRotate90)