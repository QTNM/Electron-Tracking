#define __STDCPP_WANT_MATH_SPEC_FUNCS__ 1
#include <iostream>
#include <cmath>

namespace constants{
  constexpr double pi = std::acos(-1);
  constexpr double mu0 = 4.0e-7 * pi;
}

class qtnm_field {
  double radius, current;
  double z1, z2;
  double background;
  double b_central;
public:
  void set_values(double, double, double, double, double);
  void evaluate_coil(double, double, double, double, double[]);
  void evaluate_field(double, double, double, double[]);
};

void qtnm_field::set_values(double _r, double _I, double _z1, double _z2, double _b) {
  radius = _r;
  current = _I;
  z1 = _z1;
  z2 = _z2;
  background = _b;

  // Set secondary values here
  b_central = current * constants::mu0 / radius / 2.0;
}

void qtnm_field::evaluate_field(double x, double y, double z, double field[3]) {

  double field1[3];
  double field2[3];
  evaluate_coil(x, y, z, z1, field1);
  evaluate_coil(x, y, z, z2, field2);
  field[0] = field1[0]+ field2[0];
  field[1] = field1[1] + field2[1];
  field[2] = field1[2] + field2[2] + background;
}

void qtnm_field::evaluate_coil(double x, double y, double z, double zcoil, double field[3]) {

  double rad = sqrt(pow(x,2) + pow(y,2));

  if (rad / radius < 1e-10) {
    double radius2 = pow(radius,2);
    field[0] = 0.0;
    field[1] = 0.0;
    field[2] = constants::mu0 * current * radius2 / (2.0 * pow(radius2 + pow(zcoil - z,2), 1.5));
    return;
  }

  double z_rel = z - zcoil;
  double rad_norm = rad / radius;
  double rad_norm2 = pow(rad_norm, 2);
  double z_norm2 = pow(z_rel / radius, 2);

  double alpha = pow(1.0 + rad_norm, 2) + z_norm2;
  double root_alpha_pi = sqrt(alpha) * constants::pi;
  double root_beta = sqrt(4.0 * rad_norm / alpha);
  double int_e = std::comp_ellint_2(root_beta);
  double int_k = std::comp_ellint_1(root_beta);
  double gamma = alpha - 4.0 * rad_norm;
  double b_r = b_central * (int_e * ((1.0 + rad_norm2 + z_norm2) / gamma) - int_k) / root_alpha_pi * (z_rel / rad);
  double b_z = b_central * (int_e * ((1.0 - rad_norm2 - z_norm2) / gamma) + int_k) / root_alpha_pi;

  field[0] = b_r * x / rad;
  field[1] = b_r * y / rad;
  field[2] = b_z;
}

qtnm_field field;

extern "C" {
  void set_values(double r, double I, double z1, double z2, double background) {
    field.set_values(r, I, z1, z2, background);
  }

  void calculate_field(double x, double y, double z, double bfield[3]){
    field.evaluate_field(x, y, z, bfield);
  }
}
