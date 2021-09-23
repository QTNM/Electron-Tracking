#ifndef QTNMBathtub_h
#define QTNMBathtub 1

#include <iostream>
#include <cmath>

#ifdef HAVE_BOOST
#include <boost/math/special_functions/ellint_1.hpp>
#include <boost/math/special_functions/ellint_2.hpp>
#else
#define __STDCPP_WANT_MATH_SPEC_FUNCS__ 1
#endif

namespace constants{
  constexpr double pi = 3.141592653589793238462643383279502884;
  constexpr double mu0 = 4.0e-7 * pi;
}

class QTNMField {
  double radius_, current_;
  double z1_, z2_;
  double background_;
  double b_central_;
  void EvaluateCoil(double, double, double, double, double[3]);
public:
  QTNMField(double, double, double, double, double);
  void EvaluateField(double, double, double, double[3]);
};
#endif
