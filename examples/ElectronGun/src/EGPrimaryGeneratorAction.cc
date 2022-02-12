// us
#include "EGPrimaryGeneratorAction.hh"

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
#include "Randomize.hh"
#include "G4RandomTools.hh"


EGPrimaryGeneratorAction::EGPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction()
, fParticleGun(nullptr)
, fMessenger(nullptr)
, fMean(18.575)
, fStdev(5.e-4)
, fSpot(0.5)
{
  G4int nofParticles = 1;
  fParticleGun       = new G4ParticleGun(nofParticles);

  auto particleTable = G4ParticleTable::GetParticleTable();

  // default electron particle kinematics
  fParticleGun->SetParticleDefinition(particleTable->FindParticle("e-"));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.)); // z-axis

  DefineCommands();
}

EGPrimaryGeneratorAction::~EGPrimaryGeneratorAction()
{
  delete fMessenger;
  delete fParticleGun;
}

void EGPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume 
  // from G4LogicalVolumeStore: assumes name is World_log!
  //
  auto worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World_log");
  G4Box* worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid());
  G4double worldZHalfLength = worldBox->GetZHalfLength();

  // random spot location [mm]
  G4TwoVector loc = G4RandomPointInEllipse(fSpot, fSpot); // circle
  fParticleGun->SetParticlePosition(G4ThreeVector(loc.x()*mm, loc.y()*mm, -worldZHalfLength + 1.*cm));

  // Gaussian random energy [keV]
  G4double rndEnergy = G4RandGauss(fMean, fStdev);
  fParticleGun->SetParticleEnergy(rndEnergy * keV);

  fParticleGun->GeneratePrimaryVertex(event);
}


void EGPrimaryGeneratorAction::DefineCommands()
{
  // Define /EG/generator command directory using generic messenger class
  fMessenger =
    new G4GenericMessenger(this, "/EG/generator/", "Primary generator control");

  // depth command
  auto& energyCmd = fMessenger->DeclareProperty("energy", fMean,
                                               "Mean Gun energy [keV].");
  energyCmd.SetParameterName("d", true);
  energyCmd.SetRange("d>=1.");
  energyCmd.SetDefaultValue("18.575");

  // width command
  auto& widthCmd = fMessenger->DeclareProperty("width", fStdev,
                                               "Gun energy standard deviation [keV].");
  widthCmd.SetParameterName("w", true);
  widthCmd.SetRange("w>=0.");
  widthCmd.SetDefaultValue("5.e-4");

  // width command
  auto& spotCmd = fMessenger->DeclareProperty("spot", fSpot,
                                               "Gun spot diameter [mm].");
  spotCmd.SetParameterName("s", true);
  spotCmd.SetRange("s>=0.");
  spotCmd.SetDefaultValue("0.5");
}
