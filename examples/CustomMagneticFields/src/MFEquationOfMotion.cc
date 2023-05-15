#include "MFEquationOfMotion.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

MFEquationOfMotion::MFEquationOfMotion(G4MagneticField *magField)
  : G4Mag_EqRhs(magField)
{
}

MFEquationOfMotion::~MFEquationOfMotion()
{
}

void MFEquationOfMotion::SetChargeMomentumMass(G4double particleCharge,
					       G4double MomentumXc,
					       G4double mass)
{
  fCof_val = particleCharge*eplus*c_light ; //  B must be in Tesla
  fMass = mass;
}
