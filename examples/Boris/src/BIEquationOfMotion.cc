#include "BIEquationOfMotion.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

BIEquationOfMotion::BIEquationOfMotion(G4MagneticField *magField)
  : G4Mag_EqRhs(magField)
{
}

BIEquationOfMotion::~BIEquationOfMotion()
{
}

void BIEquationOfMotion::SetChargeMomentumMass(G4double particleCharge,
					       G4double MomentumXc,
					       G4double mass)
{
  fCof_val = particleCharge*eplus*c_light ; //  B must be in Tesla
  fMass = mass;
}
