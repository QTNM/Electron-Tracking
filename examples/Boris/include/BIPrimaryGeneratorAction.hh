#ifndef BIPrimaryGeneratorAction_h
#define BIPrimaryGeneratorAction_h 1

#include "G4RootAnalysisManager.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GenericMessenger.hh"
#include "Randomize.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;


/// Primary generator
///
/// A single particle is generated.
/// macro commands can change primary properties.

class BIPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  using G4AnalysisManager = G4RootAnalysisManager;

public:
  BIPrimaryGeneratorAction();
  virtual ~BIPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event*);

private:

  void DefineCommands();

  G4ParticleGun*      fParticleGun;
  G4GenericMessenger* fMessenger;

  G4double            fMean;
  G4double            fStdev;
  G4double            fSpot;

};

#endif
