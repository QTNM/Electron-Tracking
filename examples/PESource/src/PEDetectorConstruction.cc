#include "PEDetectorConstruction.hh"

#include "G4RunManager.hh"

#include "G4Tubs.hh"
#include "G4Orb.hh"
#include "G4GeometryManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"

#include "G4SDManager.hh"
#include "PEGasSD.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

PEDetectorConstruction::PEDetectorConstruction() : G4VUserDetectorConstruction()
{
  DefineMaterials();
}

PEDetectorConstruction::~PEDetectorConstruction()
{
}

auto PEDetectorConstruction::Construct() -> G4VPhysicalVolume*
{
  return Setup(); // return world volume
}

void PEDetectorConstruction::DefineMaterials()
{
  G4NistManager* nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Galactic");
  nistManager->FindOrBuildMaterial("G4_Al");
  nistManager->FindOrBuildMaterial("G4_Ni");
  nistManager->FindOrBuildMaterial("G4_Cd");
  nistManager->FindOrBuildMaterial("G4_Au");
  nistManager->FindOrBuildMaterial("G4_Fe");
  nistManager->FindOrBuildMaterial("G4_Ag");
  nistManager->FindOrBuildMaterial("G4_Pb");
}

void PEDetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // Only need to construct the (per-thread) SD once
  if(!fSD.Get())
  {
    G4String SD1name  = "GasSD";
    PEGasSD* aGasSD = new PEGasSD(SD1name,
                                  "GasHitsCollection");
    fSD.Put(aGasSD);

    // Also only add it once to the SD manager!
    G4SDManager::GetSDMpointer()->AddNewDetector(fSD.Get());

    SetSensitiveDetector("Scorer_log", fSD.Get());
  }

}

auto PEDetectorConstruction::Setup() -> G4VPhysicalVolume*
{
  // Get materials
  auto* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  auto* plateMat      = G4Material::GetMaterial("G4_Ni"); // CHANGE THIS for different target plate
    
  // size parameter, unit [cm]
  // world
  G4double worldrad   = 60.0 * mm;    // World radius
  G4double scorerrad  = 50.0 * mm;    // Scorer radius
   
  // source structure, ring holds foils with source in between
  G4double platerad     = 12.5 * mm;    // plate diam 25 mm
  G4double platethick   = 2.5 * mm;     // plate thickness 5 mm

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
  // metal plate
  // 
  auto* plateSolid = new G4Tubs("Plate", 0.0, platerad,
				platethick, 0.0, CLHEP::twopi);
  
  //
  // logical volume
  auto* plateLogical    = new G4LogicalVolume(plateSolid, plateMat, "Plate_log");
      
  // placement
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.),
                    plateLogical, "Plate_phys", scorerLogical, false, 0, true);
    
  return worldPhysical;
}
