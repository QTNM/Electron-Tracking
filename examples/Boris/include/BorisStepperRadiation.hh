#ifndef BorisStepperRadiation_HH
#define BorisStepperRadiation_HH

#include "BorisStepper.hh"
#include "BIEquationOfMotion.hh"

class BorisStepperRadiation : public BorisStepper
{
public:

  BorisStepperRadiation(BIEquationOfMotion* EqnOfMot);
  ~BorisStepperRadiation();

  void RadiationReaction(G4ThreeVector initVelocity,
			 G4ThreeVector endVelocity);

private:

  BIEquationOfMotion* fPtrBIEqOfMot;

};

#endif
