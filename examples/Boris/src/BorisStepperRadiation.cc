#include "BorisStepperRadiation.hh"

BorisStepperRadiation::BorisStepperRadiation(BIEquationOfMotion* EqnOfMot)
  : BorisStepper(EqnOfMot),
    fPtrBIEqOfMot(EqnOfMot)
{
};

BorisStepperRadiation::~BorisStepperRadiation()
{
};


void
BorisStepperRadiation::RadiationReaction(G4ThreeVector initVelocity,
					 G4ThreeVector endVelocity)
{
  // Dummy code - do nothing
  endVelocity = initVelocity;
};
