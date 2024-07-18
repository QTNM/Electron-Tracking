// us
#include "PEPrimaryGeneratorAction.hh"

// geant
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeneralParticleSource.hh"


PEPrimaryGeneratorAction::PEPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction()
, fParticleGPS(nullptr)
{
  fParticleGPS       = new G4GeneralParticleSource();
  auto ptable        = G4ParticleTable::GetParticleTable();
  
  // simple init if macro does not define parameters
  fParticleGPS->SetParticleDefinition(ptable->FindParticle("gamma"));
  fParticleGPS->SetNumberOfParticles(1);
  fParticleGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
  fParticleGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(0.,0.,-1.0*cm)); // below origin
  fParticleGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(22.*keV); // candidate energy, say roughly Cd-109 source
  fParticleGPS->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.)); // up z-axis

}

PEPrimaryGeneratorAction::~PEPrimaryGeneratorAction()
{
  delete fParticleGPS;
}

void PEPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  // simply run the general particle source, all commands from macro
  fParticleGPS->GeneratePrimaryVertex(event);
}
