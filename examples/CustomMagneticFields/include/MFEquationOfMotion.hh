#ifndef MFEquationOfMotion_HH
#define MFEquationOfMotion_HH

#include "G4Mag_EqRhs.hh"

class MFEquationOfMotion : public G4Mag_EqRhs
{
public:

  MFEquationOfMotion(G4MagneticField *magField);
  ~MFEquationOfMotion();

  void SetChargeMomentumMass(G4double particleCharge,
			     G4double MomentumXc,
			     G4double mass);

private:
  G4double fCof_val;
  G4double fMass;
};

#endif
