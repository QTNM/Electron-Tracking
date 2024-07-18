#ifndef PEPrimaryGeneratorAction_h
#define PEPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GenericMessenger.hh"
#include "globals.hh"

class G4GeneralParticleSource;
class G4Event;


/// Primary generator
///
/// A single particle is generated.
/// macro commands can change primary properties.

class PEPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PEPrimaryGeneratorAction();
  virtual ~PEPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event*);

private:

  G4GeneralParticleSource*      fParticleGPS;

};

#endif
