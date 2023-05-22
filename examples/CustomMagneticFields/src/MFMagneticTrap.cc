#include "MFMagneticTrap.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

MFMagneticTrap::MFMagneticTrap(const G4ThreeVector& FieldVector )
{
  fFieldComponents[0] = FieldVector.x();
  fFieldComponents[1] = FieldVector.y();
  fFieldComponents[2] = FieldVector.z();

  // TODO: More constructors/get/set routines for these
  radius_ = 100; // 10cm
  current_ = 1 * CLHEP::ampere; // 1 Amp
  SetCentralField();
}

void
MFMagneticTrap::SetFieldValue(const G4ThreeVector& newFieldVector )
{
  fFieldComponents[0] = newFieldVector.x();
  fFieldComponents[1] = newFieldVector.y();
  fFieldComponents[2] = newFieldVector.z();
}

void
MFMagneticTrap::SetCurrent(const G4double TrapCurrent)
{
  current_ = TrapCurrent;
  SetCentralField();
}

void
MFMagneticTrap::SetCentralField()
{
  b_central_ = current_ * CLHEP::mu0 / radius_ / 2.0;
}

MFMagneticTrap::~MFMagneticTrap()
{
}

MFMagneticTrap::MFMagneticTrap (const MFMagneticTrap &p)
  : G4MagneticField(p)
{
  for (G4int i=0; i<3; i++)
    fFieldComponents[i] = p.fFieldComponents[i];

  radius_ = p.radius_;
  current_ = p.current_;
  b_central_ = p.b_central_;
}

MFMagneticTrap& MFMagneticTrap::operator = (const MFMagneticTrap &p)
{
  if (&p == this) return *this;
  for (G4int i=0; i<3; i++)
    fFieldComponents[i] = p.fFieldComponents[i];

  radius_ = p.radius_;
  current_ = p.current_;
  b_central_ = p.b_central_;
  return *this;
}

void MFMagneticTrap::GetFieldValue (const G4double yIn[7],
				       G4double *B  ) const 
{

  G4double field[3];
  EvaluateCoil(yIn, 0.0, field);
  
  B[0]= fFieldComponents[0] + field[0] ;
  B[1]= fFieldComponents[1] + field[1] ;
  B[2]= fFieldComponents[2] + field[2] ;
}

G4ThreeVector MFMagneticTrap::GetConstantFieldValue() const
{
  G4ThreeVector B(fFieldComponents[0],
		  fFieldComponents[1],
		  fFieldComponents[2]);
  return B;
}

void MFMagneticTrap::EvaluateCoil(const G4double yIn[7], const G4double zcoil, G4double field[3]) const
{
  G4double x = yIn[0];
  G4double y = yIn[1];
  G4double z = yIn[2];

  double rad = sqrt(pow(x,2) + pow(y,2));

  if (rad / radius_ < 1e-10) {
    double radius2 = pow(radius_,2);
    field[0] = 0.0;
    field[1] = 0.0;
    field[2] = CLHEP::mu0 * current_ * radius2 / (2.0 * pow(radius2 + pow(zcoil - z,2), 1.5));
    return;
  }

  double z_rel = z - zcoil;
  double rad_norm = rad / radius_;
  double rad_norm2 = pow(rad_norm, 2);
  double z_norm2 = pow(z_rel / radius_, 2);

  double alpha = pow(1.0 + rad_norm, 2) + z_norm2;
  double root_alpha_pi = sqrt(alpha) * CLHEP::pi;
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
