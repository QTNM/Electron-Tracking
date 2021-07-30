#ifndef EGPrimaryGeneratorAction_h
#define EGPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;


/// Primary generator
///
/// A single particle is generated.
/// macro commands can change primary properties.

class EGPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  EGPrimaryGeneratorAction();
  virtual ~EGPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event*);

private:

  G4ParticleGun*      fParticleGun;

};

#endif
