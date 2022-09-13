#include "EGDetectorConstruction.hh"

#include "G4RunManager.hh"

#include "G4Box.hh"
#include "G4GeometryManager.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"

#include "G4GlobalMagFieldMessenger.hh"
#include "G4UniformMagField.hh"
#include "G4AutoDelete.hh"

#include "G4SDManager.hh"
#include "EGGasSD.hh"
#include "EGWatchSD.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

EGDetectorConstruction::EGDetectorConstruction()
{
  DefineCommands();
}

EGDetectorConstruction::~EGDetectorConstruction()
{

  delete fDetectorMessenger;

}

auto EGDetectorConstruction::Construct() -> G4VPhysicalVolume*
{
  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  DefineMaterials();

  return SetupShort();

}

void EGDetectorConstruction::DefineMaterials()
{
  G4NistManager* nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Galactic");
  nistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL");

}

void EGDetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // Only need to construct the (per-thread) SD once
  if(!fSD1.Get())
  {
    G4String SD1name  = "GasSD";
    EGGasSD* aGasSD = new EGGasSD(SD1name,
                                  "GasHitsCollection");
    fSD1.Put(aGasSD);

    G4String SD2name  = "WatchSD";
    EGWatchSD* aWatchSD = new EGWatchSD(SD2name,
                                        "WatchHitsCollection");
    fSD2.Put(aWatchSD);

    // Also only add it once to the SD manager!
    G4SDManager::GetSDMpointer()->AddNewDetector(fSD1.Get());
    G4SDManager::GetSDMpointer()->AddNewDetector(fSD2.Get());

    SetSensitiveDetector("Gas_log", fSD1.Get());
    SetSensitiveDetector("Stop_log", fSD2.Get());
  }

  // Field setup
  if( !fFieldMessenger.Get() ) {
    // Create global magnetic field messenger.
    // Uniform magnetic field is then created automatically if
    // the field value is not zero.
    G4ThreeVector fieldValue = G4ThreeVector();
    G4GlobalMagFieldMessenger* msg =
      new G4GlobalMagFieldMessenger(fieldValue);
    msg->SetVerboseLevel(1);
    G4AutoDelete::Register(msg);
    fFieldMessenger.Put( msg );
  }

}

auto EGDetectorConstruction::SetupShort() -> G4VPhysicalVolume*
{
  // Get materials
  auto* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  auto* steelMat      = G4Material::GetMaterial("G4_STAINLESS-STEEL");
  auto* gasMat        = G4Material::GetMaterial("G4_Galactic");
    
  // size parameter, unit [cm]
  // world
  G4double worldhside = 10.0 * cm;   // box half side in x, y
  G4double worldhZ    = 0.51 * m;    // box half side in z; full 1.02 metre
   
  // tubes with Gas ROI
  G4double pipewall  = 0.1 * cm;    // tube thickness 1 mm
  G4double piperad   = 5.0 * cm;    // tube diam 10 cm
  G4double pipehZ    = worldhZ - 1*cm;  // fit into world 1cm either side=1m

  // stopwatch volume with piperad and thickness in z
  G4double heightZ   = 0.1 * cm;   // 2 mm thick in z
 
  // Volumes for this geometry
  
  //
  // World
  //
  auto* worldSolid = new G4Box("World", worldhside, worldhside, worldhZ);
  auto* worldLogical  = new G4LogicalVolume(worldSolid, worldMaterial, "World_log");
  auto* worldPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), worldLogical,
                                           "World_phys", nullptr, false, 0);
  
  //
  // pipe, hollow cylinder shell
  // 
  auto* pipeSolid = new G4Tubs("Pipe", piperad, piperad + pipewall,
                                 pipehZ, 0.0, CLHEP::twopi);
  
  //
  // Target gas, solid cylinder
  // 
  auto* gasSolid = new G4Tubs("Gas", 0.0 * cm, piperad, pipehZ,
                               0.0, CLHEP::twopi);

  // stopwatch disk at end of pipe
  auto* stopSolid = new G4Tubs("Stop", 0.0 * cm, piperad, heightZ,
                                0.0, CLHEP::twopi);
  
  //
  // logical volumes
  auto* pipeLogical = new G4LogicalVolume(pipeSolid, worldMaterial, "Pipe_log");
  auto* gasLogical  = new G4LogicalVolume(gasSolid, gasMat, "Gas_log");
  auto* stopLogical = new G4LogicalVolume(stopSolid, worldMaterial, "Stop_log");

    
  // placements
  new G4PVPlacement(nullptr, G4ThreeVector(0. * cm, 0. * cm, 0. * cm),
                    pipeLogical, "Pipe_phys", worldLogical, false, 0, true);
  
  new G4PVPlacement(nullptr, G4ThreeVector(0. * cm, 0. * cm, 0. * cm), gasLogical,
                    "Gas_phys", worldLogical, false, 0, true); 

  new G4PVPlacement(nullptr, G4ThreeVector(0. * cm, 0. * cm, pipehZ + heightZ), stopLogical,
                    "Stop_phys", worldLogical, false, 0, true);

  return worldPhysical;
}

void EGDetectorConstruction::DefineCommands()
{
  // Define geometry command directory using generic messenger class
  fDetectorMessenger = new G4GenericMessenger(this, "/EG/detector/",
                                              "Commands for controlling detector setup");
}
