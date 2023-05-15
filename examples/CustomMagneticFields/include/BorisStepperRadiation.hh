#ifndef BorisStepperRadiation_HH
#define BorisStepperRadiation_HH

#include "BorisStepper.hh"
#include "MFEquationOfMotion.hh"

class BorisStepperRadiation : public BorisStepper
{
public:

  BorisStepperRadiation(MFEquationOfMotion* EqnOfMot);
  ~BorisStepperRadiation();

  void RadiationReaction(G4ThreeVector initVelocity,
			 G4ThreeVector endVelocity);

private:

  MFEquationOfMotion* fPtrMFEqOfMot;

};

#endif
