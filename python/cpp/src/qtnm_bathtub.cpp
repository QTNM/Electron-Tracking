#include "qtnm_bathtub.h"

QTNMField::QTNMField(double radius, double current, double z1, double z2, double background) :
  radius_(radius),
  current_(current),
  z1_(z1),
  z2_(z2),
  background_(background)
{
  b_central_ = current_ * constants::mu0 / radius_ / 2.0;
}

void QTNMField::EvaluateField(double x, double y, double z, double field[3]) {

  double field1[3];
  double field2[3];
  EvaluateCoil(x, y, z, z1_, field1);
  EvaluateCoil(x, y, z, z2_, field2);
  field[0] = field1[0] + field2[0];
  field[1] = field1[1] + field2[1];
  field[2] = field1[2] + field2[2] + background_;
}

void QTNMField::EvaluateCoil(double x, double y, double z, double zcoil, double field[3]) {

  double rad = sqrt(pow(x,2) + pow(y,2));

  if (rad / radius_ < 1e-10) {
    double radius2 = pow(radius_,2);
    field[0] = 0.0;
    field[1] = 0.0;
    field[2] = constants::mu0 * current_ * radius2 / (2.0 * pow(radius2 + pow(zcoil - z,2), 1.5));
    return;
  }

  double z_rel = z - zcoil;
  double rad_norm = rad / radius_;
  double rad_norm2 = pow(rad_norm, 2);
  double z_norm2 = pow(z_rel / radius_, 2);

  double alpha = pow(1.0 + rad_norm, 2) + z_norm2;
  double root_alpha_pi = sqrt(alpha) * constants::pi;
  double root_beta = sqrt(4.0 * rad_norm / alpha);
#ifdef HAVE_BOOST
  double int_e = boost::math::ellint_2(root_beta);
  double int_k = boost::math::ellint_1(root_beta);
#else
  double int_e = std::comp_ellint_2(root_beta);
  double int_k = std::comp_ellint_1(root_beta);
#endif
  double gamma = alpha - 4.0 * rad_norm;
  double b_r = b_central_ * (int_e * ((1.0 + rad_norm2 + z_norm2) / gamma) - int_k) / root_alpha_pi * (z_rel / rad);
  double b_z = b_central_ * (int_e * ((1.0 - rad_norm2 - z_norm2) / gamma) + int_k) / root_alpha_pi;

  field[0] = b_r * x / rad;
  field[1] = b_r * y / rad;
  field[2] = b_z;
}

extern "C" {
  QTNMField* new_field(double r, double I, double z1, double z2, double background) {
    return new QTNMField(r, I, z1, z2, background);
  }

  void delete_field(QTNMField* field) {
    delete field;
  }

  void calculate_field(QTNMField* field, double x, double y, double z, double bfield[3]){
    field->EvaluateField(x, y, z, bfield);
  }
}
