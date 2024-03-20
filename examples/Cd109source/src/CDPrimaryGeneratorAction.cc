// us
#include "CDPrimaryGeneratorAction.hh"

// geant
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4IonTable.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4RandomTools.hh"


CDPrimaryGeneratorAction::CDPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction()
, fParticleGun(nullptr)
{
  G4int nofParticles = 1;
  fParticleGun       = new G4ParticleGun(nofParticles);

  // default ion particle kinematics
  fParticleGun->SetParticleEnergy(0.*keV);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.)); // z-axis

}

CDPrimaryGeneratorAction::~CDPrimaryGeneratorAction()
{
  delete fParticleGun;
}

void CDPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume 
  // from G4LogicalVolumeStore: assumes name is World_log!
  //
  // auto worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World_log");

  // set Cd-109 as default ion
  G4int Z = 48;
  G4int A = 109;
  G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z,A,0.*keV);
  fParticleGun->SetParticleDefinition(ion);
  fParticleGun->SetParticleCharge(0.*eplus);
  
  // random spot location [mm]
  G4double fSpot = 5.0*mm; // fixed spot diameter for each source
  G4TwoVector loc = G4RandomPointInEllipse(fSpot/2.0, fSpot/2.0); // circle
  fParticleGun->SetParticlePosition(G4ThreeVector(loc.x()/mm, loc.y()/mm, 0./mm)); // at z=0

  fParticleGun->GeneratePrimaryVertex(event);
}
