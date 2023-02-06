#ifndef BIEquationOfMotion_HH
#define BIEquationOfMotion_HH

#include "G4Mag_EqRhs.hh"

class BIEquationOfMotion : public G4Mag_EqRhs
{
public:

  BIEquationOfMotion(G4MagneticField *magField);
  ~BIEquationOfMotion();

  void SetChargeMomentumMass(G4double particleCharge,
			     G4double MomentumXc,
			     G4double mass);

private:
  G4double fCof_val;
  G4double fMass;
};

#endif
