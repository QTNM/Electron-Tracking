#ifndef CDPrimaryGeneratorAction_h
#define CDPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GenericMessenger.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "CLHEP/Random/Random.h"
#include "CDDetectorConstruction.hh"
class G4ParticleGun;
class G4Event;


/// Primary generator
///
/// A single particle is generated.
/// macro commands can change primary properties.

class CDPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  CDPrimaryGeneratorAction(CDDetectorConstruction* detector);
  virtual ~CDPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event*);

private:

  G4ParticleGun*      fParticleGun;
  CLHEP::HepRandom*   rndmNumberGenerator; 
  G4double randomRadiusInShell();
  CDDetectorConstruction* _detector;
};

#endif
