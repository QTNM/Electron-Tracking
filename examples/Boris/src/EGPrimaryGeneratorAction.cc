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
#include "G4RandomTools.hh"
#include "g4root.hh"

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
  G4TwoVector loc = G4RandomPointInEllipse(fSpot/2.0, fSpot/2.0); // circle
  fParticleGun->SetParticlePosition(G4ThreeVector(loc.x()*mm, loc.y()*mm, -worldZHalfLength + 1.*cm));

  // Gaussian random energy [keV]
  G4double en = G4RandGauss::shoot(fMean, fStdev);
  fParticleGun->SetParticleEnergy(en * keV);

  // Random direction
  G4double theta_max = 0.07; // ~+/- 4 degrees
  G4double theta = G4UniformRand() *2.0 * theta_max - theta_max;
  G4double cosTheta = std::cos(theta), sinTheta = std::sqrt(1.0 - cosTheta*cosTheta);
  G4double cosPhi = std::cos(twopi*G4UniformRand()), sinPhi = std::sqrt(1.0 - cosPhi*cosPhi);
  G4double ux = sinTheta*cosPhi,
           uy = sinTheta*sinPhi,
           uz = cosTheta;

  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(ux,uy,uz)); // z-axis

  fParticleGun->GeneratePrimaryVertex(event);

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Initial Conditions
  analysisManager->FillNtupleIColumn(2, 0, event->GetEventID()); // repeat all rows
  analysisManager->FillNtupleIColumn(2, 1, 0);
  analysisManager->FillNtupleDColumn(2, 2, 0.0);
  analysisManager->FillNtupleDColumn(2, 3, loc.x());
  analysisManager->FillNtupleDColumn(2, 4, loc.y());
  analysisManager->FillNtupleDColumn(2, 5, 0.0);
  analysisManager->FillNtupleDColumn(2, 6, fParticleGun->GetParticleMomentumDirection().x());
  analysisManager->FillNtupleDColumn(2, 7, fParticleGun->GetParticleMomentumDirection().y());
  analysisManager->FillNtupleDColumn(2, 8, fParticleGun->GetParticleMomentumDirection().z());
  analysisManager->FillNtupleDColumn(2, 9, en);
  analysisManager->AddNtupleRow(2);
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
