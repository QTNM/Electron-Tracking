#include "BorisStepperRadiation.hh"

BorisStepperRadiation::BorisStepperRadiation(MFEquationOfMotion* EqnOfMot)
  : BorisStepper(EqnOfMot),
    fPtrMFEqOfMot(EqnOfMot)
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
