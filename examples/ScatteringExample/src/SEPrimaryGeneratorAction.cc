// us
#include "SEPrimaryGeneratorAction.hh"

// geant
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"

SEPrimaryGeneratorAction::SEPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction()
, fParticleGun(nullptr)
{
  G4int nofParticles = 1;
  fParticleGun       = new G4ParticleGun(nofParticles);

  auto particleTable = G4ParticleTable::GetParticleTable();

  // default particle kinematics
  fParticleGun->SetParticleDefinition(particleTable->FindParticle("e-"));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.)); // z-axis
  fParticleGun->SetParticleEnergy(18.6*keV);
}

SEPrimaryGeneratorAction::~SEPrimaryGeneratorAction()
{
  delete fParticleGun;
}

void SEPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume 
  // from G4LogicalVolumeStore: assumes name is World!
  //
  auto worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World");
  G4Box* worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid());
  G4double worldZHalfLength = worldBox->GetZHalfLength();
  fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, -worldZHalfLength));

  fParticleGun->GeneratePrimaryVertex(event);
}

