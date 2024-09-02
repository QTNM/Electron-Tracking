// us
#include "CDPrimaryGeneratorAction.hh"
#include "CDDetectorConstruction.hh"
#include "CDActionInitialization.hh"

//maths
#include <iostream>
#include <cmath>

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


CDPrimaryGeneratorAction::CDPrimaryGeneratorAction(CDDetectorConstruction* detector)
: G4VUserPrimaryGeneratorAction()
, fParticleGun(nullptr)
, _detector(detector)
{
  G4int nofParticles = 1;
  fParticleGun       = new G4ParticleGun(nofParticles);

  // default ion particle kinematics
  fParticleGun->SetParticleEnergy(0.*keV);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.)); // z-axis

  // Create random number generator:
  rndmNumberGenerator = new CLHEP::HepRandom();
}

CDPrimaryGeneratorAction::~CDPrimaryGeneratorAction()
{
  delete fParticleGun;
}

void CDPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{

  // Test random number generator:
  double myRndm = rndmNumberGenerator->flat();
  //std::cout << "Random number:" << myRndm << std::endl;

  // set Cd-109 as default ion
  G4int Z = 48;
  G4int A = 109;
  G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z,A,0.*keV);
  fParticleGun->SetParticleDefinition(ion);
  fParticleGun->SetParticleCharge(0.*eplus);


 G4String detectorType = _detector->DetectorType();

    if (detectorType == "Isotrak" || detectorType == "QSA") {
        // Pancake-shape source geometry (for Isotrak and QSA)

        G4double fSpot = 5.0*mm; // Fixed spot diameter for each source

        // Generate a random point in a circle
        G4TwoVector spot = G4RandomPointInEllipse(fSpot/2.0, fSpot/2.0); // circle
        G4ThreeVector loc(spot.x(), spot.y(), 0.0); // at z = 0

        fParticleGun->SetParticlePosition(G4ThreeVector(loc.x()/mm, loc.y()/mm, 0)); // at z=0
        fParticleGun->GeneratePrimaryVertex(event); // Generate primary vertex

    } else if (detectorType == "Pointlike" || detectorType == "Shell"){
        // Spherical-shape source geometry (for Pointlike and Shell)

        double rmax3 = std::pow(_detector->_r_max, 3);
        double rmin3 = std::pow(_detector->_r_min, 3);
        double m = (rmax3 - rmin3) * myRndm + rmin3;
        double r = std::pow(m, 1.0/3.0); // Randomly generated radius

        // Generate a random point on a sphere with radius r
        G4ThreeVector loc = G4RandomPointOnEllipsoid(r, r, r);

        fParticleGun->SetParticlePosition(G4ThreeVector(loc.x()/mm, loc.y()/mm, loc.z()/mm)); // at z=0
        fParticleGun->GeneratePrimaryVertex(event); // Generate primary vertex
    }

}
