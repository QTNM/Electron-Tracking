#include "CDDetectorConstruction.hh"

#include "G4RunManager.hh"

#include "G4Tubs.hh"
#include "G4Orb.hh"
#include "G4GeometryManager.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4PVPlacement.hh"
#include "G4Sphere.hh"
#include "G4ios.hh"


#include "G4SDManager.hh"
#include "CDGasSD.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

CDDetectorConstruction::CDDetectorConstruction() : G4VUserDetectorConstruction()
						 , fSource("Isotrak")
{
  DefineCommands();
  DefineMaterials();
}

CDDetectorConstruction::~CDDetectorConstruction()
{

  delete fDetectorMessenger;

}

G4String CDDetectorConstruction::DetectorType()
{

  return fSource;

}

auto CDDetectorConstruction::Construct() -> G4VPhysicalVolume*
{
  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

// Command for the detector selection
  if (fSource=="Isotrak")
    return SetupIsotrak();
  else if (fSource=="QSA")
    return SetupQSA();
  else if (fSource=="Pointlike"){
    _r_min = 0.0;  // Solid sphere has a inner radius = 0
    _r_max = 0.01*mm;  // The radius of the pointlike source
    return SetupPointlike();
  }
  else if (fSource=="Shell"){
    _r_min = 0.05*mm;  // radius of inner sphere
    _r_max = _r_min + 0.001*mm; // outer radius of the source
    return SetupShell();
  }
  else
    G4cout << "Unknown source type " << fSource << ". Defaulting to Isotrak." << G4endl;
    return SetupIsotrak();
}

void CDDetectorConstruction::DefineMaterials()
{
  G4NistManager* nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Galactic");
  nistManager->FindOrBuildMaterial("G4_Al");
  nistManager->FindOrBuildMaterial("G4_Ni");
  nistManager->FindOrBuildMaterial("G4_Cd");
  nistManager->FindOrBuildMaterial("G4_MYLAR");
  nistManager->FindOrBuildMaterial("G4_LUCITE"); // Acrylic
}

void CDDetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // Only need to construct the (per-thread) SD once
  if(!fSD.Get())
  {
    G4String SD1name  = "GasSD";
    CDGasSD* aGasSD = new CDGasSD(SD1name,
                                  "GasHitsCollection");
    fSD.Put(aGasSD);

    // Also only add it once to the SD manager!
    G4SDManager::GetSDMpointer()->AddNewDetector(fSD.Get());

    SetSensitiveDetector("Scorer_log", fSD.Get());
  }

}

auto CDDetectorConstruction::SetupQSA() -> G4VPhysicalVolume*
{

   G4cout << "Setting up detector SetupQSA" << G4endl;

  // Get materials
  auto* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  auto* foilMat       = G4Material::GetMaterial("G4_LUCITE");
  auto* ringMat       = G4Material::GetMaterial("G4_Al");
  auto* sourceMat     = G4Material::GetMaterial("G4_Ni");

  // size parameter, unit [cm]
  // world
  G4double worldrad   = 50.0 * mm;    // Orb radius
  G4double scorerrad  = 30.0 * mm;    // Dome radius

  // source structure, ring holds foils with source in between
  G4double ringrad     = 12.5 * mm;    // ring diam 25 mm
  G4double ringthick   = 1.5 * mm;     // ring thickness 3 mm

  G4double foilrad     = 11.0 * mm;    // foil diam 22 mm
  G4double foilthick   =  0.001 * mm;    // foil thickness   2 um
  G4double sourcethick =  0.05 * mm;    // sample backing thickness 0.1 mm

  // Volumes for this geometry

  //
  // World at origin
  //
  auto* worldSolid = new G4Orb("World", worldrad);
  auto* worldLogical  = new G4LogicalVolume(worldSolid, worldMaterial, "World_log");
  auto* worldPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), worldLogical,
                                           "World_phys", nullptr, false, 0);
  //
  // Scorer orb inside world
  //
  auto* scorerSolid = new G4Orb("Scorer", scorerrad);
  auto* scorerLogical  = new G4LogicalVolume(scorerSolid, worldMaterial, "Scorer_log");
  auto* scorerPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), scorerLogical,
                                           "Scorer_phys", worldLogical, false, 0, true);

  //
  // Alu ring
  //
  auto* ringSolid = new G4Tubs("Ring", foilrad, ringrad,
			       ringthick, 0.0, CLHEP::twopi);

  //
  // Acrylic/Lucite foil and source layer
  //
  auto* foilSolid   = new G4Tubs("Foil", 0.0, foilrad, foilthick,
				 0.0, CLHEP::twopi);
  auto* sourceSolid = new G4Tubs("Source", 0.0, foilrad, sourcethick,
				 0.0, CLHEP::twopi);

  //
  // logical volumes
  auto* ringLogical    = new G4LogicalVolume(ringSolid, ringMat, "Ring_log");
  auto* foilLogical    = new G4LogicalVolume(foilSolid, foilMat, "Foil_log");
  auto* sourceLogical  = new G4LogicalVolume(sourceSolid, sourceMat, "Source_log");

  // placements
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.),
                    ringLogical, "Ring_phys", scorerLogical, false, 0, true);
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,foilthick), foilLogical,
                    "Foil_phys", scorerLogical, false, 0, true);
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,-sourcethick), sourceLogical,
                    "Source_phys", scorerLogical, false, 0, true);

  return worldPhysical;
}


auto CDDetectorConstruction::SetupIsotrak() -> G4VPhysicalVolume*
{

   G4cout << "Setting up detector SetupIsotrak" << G4endl;

  // Get materials
  auto* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  auto* foilMat       = G4Material::GetMaterial("G4_MYLAR");
  auto* ringMat       = G4Material::GetMaterial("G4_Al");
  auto* sourceMat     = G4Material::GetMaterial("G4_Cd");

  // size parameter, unit [cm]
  // world
  G4double worldrad   = 50.0 * mm;    // Orb radius
  G4double scorerrad  = 30.0 * mm;    // Dome radius

  // source structure, ring holds foils with source in between
  G4double ringrad     = 12.5 * mm;    // ring diam 25 mm
  G4double ringthick   = 1.5 * mm;     // ring thickness 3 mm

  G4double foilrad     = 11.0 * mm;    // foil diam 22 mm
  G4double foilthick   =  0.003 * mm;    // foil thickness   6 um
  G4double sourcethick =  5.e-5 * mm;    // source thickness 0.1 um

  // Volumes for this geometry

  //
  // World at origin
  //
  auto* worldSolid = new G4Orb("World", worldrad);
  auto* worldLogical  = new G4LogicalVolume(worldSolid, worldMaterial, "World_log");
  auto* worldPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), worldLogical,
                                           "World_phys", nullptr, false, 0);

  //
  // Scorer orb inside world
  //
  auto* scorerSolid = new G4Orb("Scorer", scorerrad);
  auto* scorerLogical  = new G4LogicalVolume(scorerSolid, worldMaterial, "Scorer_log");
  auto* scorerPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), scorerLogical,
                                           "Scorer_phys", worldLogical, false, 0, true);

  //
  // Alu ring
  //
  auto* ringSolid = new G4Tubs("Ring", foilrad, ringrad,
			       ringthick, 0.0, CLHEP::twopi);

  //
  // Mylar foil and source layer
  //
  auto* foilSolid   = new G4Tubs("Foil", 0.0, foilrad, foilthick,
				 0.0, CLHEP::twopi);
  auto* sourceSolid = new G4Tubs("Source", 0.0, foilrad, sourcethick,
				 0.0, CLHEP::twopi);

  //
  // logical volumes
  auto* ringLogical    = new G4LogicalVolume(ringSolid, ringMat, "Ring_log");
  auto* foilLogical    = new G4LogicalVolume(foilSolid, foilMat, "Foil_log");
  auto* sourceLogical  = new G4LogicalVolume(sourceSolid, sourceMat, "Source_log");

  // placements
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.),
                    ringLogical, "Ring_phys", scorerLogical, false, 0, true);

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,-foilthick-sourcethick), foilLogical,
                    "Foil_bot_phys", scorerLogical, false, 0, true);
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), sourceLogical, // around origin
                    "Source_phys", scorerLogical, false, 0, true);
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,foilthick+sourcethick), foilLogical,
                    "Foil_top_phys", scorerLogical, false, 1, true);

  return worldPhysical;
}




//This would be the function for a pointlike source
auto CDDetectorConstruction::SetupPointlike() -> G4VPhysicalVolume*
{

  G4cout << "Setting up detector SetupPointlike" << G4endl;

  //this function is used to modify a pointlike source
  // Get materials
  auto* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  auto* sourceMat     = G4Material::GetMaterial("G4_Cd");

  // size parameter, unit [cm]
  // world
  G4double worldrad   = 50.0 * mm;    // Orb radius
  G4double scorerrad  = 30.0 * mm;    // Dome radius


  //
  // World at origin
  //
  auto* worldSolid = new G4Orb("World", worldrad);
  auto* worldLogical  = new G4LogicalVolume(worldSolid, worldMaterial, "World_log");
  auto* worldPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), worldLogical,
                                           "World_phys", nullptr, false, 0);

  //
  // Scorer orb inside world
  //
  auto* scorerSolid = new G4Orb("Scorer", scorerrad);
  auto* scorerLogical  = new G4LogicalVolume(scorerSolid, worldMaterial, "Scorer_log");
  auto* scorerPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), scorerLogical,
                                           "Scorer_phys", worldLogical, false, 0, true);


  //Geometry of the pointlike source
  auto* sourceSolid = new G4Sphere("Source", 0, _r_max, 0.0, CLHEP::twopi,
				 0.0, CLHEP::pi);  //point-like (solod sphere) source with very small radius ==  10 um

  //
  // logical volumes
  auto* sourceLogical  = new G4LogicalVolume(sourceSolid, sourceMat, "Source_log");

  // placements
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), sourceLogical, // around origin
                    "Source_phys", scorerLogical, false, 0, true);


  return worldPhysical;
}




//This would be the function for a shell source on a steel solid sphere
auto CDDetectorConstruction::SetupShell() -> G4VPhysicalVolume*
{

  G4cout << "Setting up detector SetupShell" << G4endl;
  G4cout << "Generating random point in shell between r_min = " << _r_min << " and r_max = " << _r_max << G4endl;

  //this function is used to modify a pointlike source
  // Get materials
  auto* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  auto* baseMat       = G4Material::GetMaterial("G4_Al");
  auto* sourceMat     = G4Material::GetMaterial("G4_Cd");

  //Scorer sphere data
  G4double scorerrad  = 30.0 * mm;    // Dome radius
  G4double worldrad   = 50.0 * mm;    // Orb radius
  //G4cout<< worldrad << G4endl;
  // Volumes for this geometry

  //
  // World at origin
  //
  auto* worldSolid = new G4Orb("World", worldrad);
  auto* worldLogical  = new G4LogicalVolume(worldSolid, worldMaterial, "World_log");
  auto* worldPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), worldLogical,
                                           "World_phys", nullptr, false, 0);

  //
  // Scorer orb inside world
  //
  auto* scorerSolid = new G4Orb("Scorer", scorerrad);
  auto* scorerLogical  = new G4LogicalVolume(scorerSolid, worldMaterial, "Scorer_log");
  auto* scorerPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), scorerLogical,
                                           "Scorer_phys", worldLogical, false, 0, true);

  //

//geometry for the shell and solid sphere
  //inner solid sphere for Alu
  auto* baseSolid = new G4Sphere("Source", 0, _r_min, 0.0, CLHEP::twopi,
				 0.0, CLHEP::pi);


  //outer sphere shell for cd109
  auto* sourceSolid = new G4Sphere("Source", _r_min, _r_max, 0.0, CLHEP::twopi,
				 0.0, CLHEP::pi);


  // logical volumes

  auto* baseLogical    = new G4LogicalVolume(baseSolid, baseMat, "base_log");
  auto* sourceLogical  = new G4LogicalVolume(sourceSolid, sourceMat, "Source_log");

  // placements
 // new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.),
                    //ringLogical, "Ring_phys", scorerLogical, false, 0, true);

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0), baseLogical,
                    "base_bot_phys", scorerLogical, false, 0, true);
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), sourceLogical, // around origin
                    "Source_phys", scorerLogical, false, 0, true);
  //new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,foilthick+sourcethick), foilLogical,
                    //"Foil_top_phys", scorerLogical, false, 1, true);

  return worldPhysical;
}




void CDDetectorConstruction::Switch(G4String choice)
{
  fSource = choice;

  // Reinit wiping out stores
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}


void CDDetectorConstruction::DefineCommands()
{
  // Define geometry command directory using generic messenger class
  fDetectorMessenger = new G4GenericMessenger(this, "/CD/source/",
                                              "Commands for controlling source setup");

  // switch command
  fDetectorMessenger->DeclareMethod("choice", &CDDetectorConstruction::Switch)
    .SetGuidance("Set source choice string: Isotrak; QSA; Pointlike; Shell.")
    .SetStates(G4State_PreInit)
    .SetToBeBroadcasted(false);
}
