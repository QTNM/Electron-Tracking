#include "CDDetectorConstruction.hh"

#include "G4RunManager.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
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

#include "G4SDManager.hh"
#include "CDGasSD.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

CDDetectorConstruction::CDDetectorConstruction() : G4VUserDetectorConstruction()
						 , fSource(true)
{
  DefineCommands();
  DefineMaterials();
}

CDDetectorConstruction::~CDDetectorConstruction()
{

  delete fDetectorMessenger;

}

auto CDDetectorConstruction::Construct() -> G4VPhysicalVolume*
{
  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  if (fSource)
    return SetupIsotrak();
  else
    return SetupQSA();
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
  // Get materials
  auto* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  auto* foilMat       = G4Material::GetMaterial("G4_LUCITE");
  auto* ringMat       = G4Material::GetMaterial("G4_Al");
  auto* sourceMat     = G4Material::GetMaterial("G4_Ni");
    
  // size parameter, unit [cm]
  // world
  G4double worldrad   = 50.0 * mm;    // Orb radius
  G4double scorerrad  = 11.0 * mm;    // Dome radius
   
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
  
  // Scorer dome, half-sphere
  auto* scorerSolid   = new G4Sphere("Score", 0.0, scorerrad,
				     0.0, 2*CLHEP::twopi, CLHEP::pi, CLHEP::twopi);

  //
  // logical volumes
  auto* ringLogical    = new G4LogicalVolume(ringSolid, ringMat, "Ring_log");
  auto* foilLogical    = new G4LogicalVolume(foilSolid, foilMat, "Foil_log");
  auto* sourceLogical  = new G4LogicalVolume(sourceSolid, sourceMat, "Source_log");
  auto* scorerLogical  = new G4LogicalVolume(scorerSolid, worldMaterial, "Scorer_log");
    
  // placements
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.),
                    ringLogical, "Ring_phys", worldLogical, false, 0, true);
  
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,foilthick), foilLogical,
                    "Foil_phys", worldLogical, false, 0, true); 
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,-sourcethick), sourceLogical,
                    "Source_phys", worldLogical, false, 0, true); 
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,2*foilthick), scorerLogical,
                    "Scorer_phys", worldLogical, false, 0, true); 
    
  return worldPhysical;
}


auto CDDetectorConstruction::SetupIsotrak() -> G4VPhysicalVolume*
{
  // Get materials
  auto* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  auto* foilMat       = G4Material::GetMaterial("G4_MYLAR");
  auto* ringMat       = G4Material::GetMaterial("G4_Al");
  auto* sourceMat     = G4Material::GetMaterial("G4_Cd");
    
  // size parameter, unit [cm]
  // world
  G4double worldrad   = 50.0 * mm;    // Orb radius
  G4double scorerrad  = 11.0 * mm;    // Dome radius
   
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
  
  // Scorer dome, half-sphere
  auto* scorerSolid   = new G4Sphere("Score", 0.0, scorerrad,
				     0.0, 2*CLHEP::twopi, CLHEP::pi, CLHEP::twopi);

  //
  // logical volumes
  auto* ringLogical    = new G4LogicalVolume(ringSolid, ringMat, "Ring_log");
  auto* foilLogical    = new G4LogicalVolume(foilSolid, foilMat, "Foil_log");
  auto* sourceLogical  = new G4LogicalVolume(sourceSolid, sourceMat, "Source_log");
  auto* scorerLogical  = new G4LogicalVolume(scorerSolid, worldMaterial, "Scorer_log");
    
  // placements
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.),
                    ringLogical, "Ring_phys", worldLogical, false, 0, true);
  
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,-foilthick-sourcethick), foilLogical,
                    "Foil_bottom_phys", worldLogical, false, 0, true); 
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), sourceLogical, // around origin
                    "Source_phys", worldLogical, false, 0, true); 
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,foilthick+sourcethick), foilLogical,
                    "Foil_top_phys", worldLogical, false, 1, true); 
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,2*foilthick+sourcethick), scorerLogical,
                    "Scorer_phys", worldLogical, false, 0, true); 
    
  return worldPhysical;
}


void CDDetectorConstruction::Switch(G4bool choice)
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
    .SetGuidance("Set source choice boolean: true=Isotrak; false=inverted.")
    .SetStates(G4State_PreInit)
    .SetToBeBroadcasted(false);
}
