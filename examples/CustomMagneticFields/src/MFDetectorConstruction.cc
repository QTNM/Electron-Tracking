#include "MFDetectorConstruction.hh"

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
#include "MFGasSD.hh"
#include "MFWatchSD.hh"
#include "MFMagneticFieldSetup.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

MFDetectorConstruction::MFDetectorConstruction()
{
  maxStep = 0.1*cm;

  DefineCommands();
}

MFDetectorConstruction::~MFDetectorConstruction()
{

  delete fDetectorMessenger;

}

auto MFDetectorConstruction::Construct() -> G4VPhysicalVolume*
{
  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  DefineMaterials();

  return SetupShort();

}

void MFDetectorConstruction::DefineMaterials()
{
  G4NistManager* nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Galactic");
  nistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL");

}

void MFDetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // Only need to construct the (per-thread) SD once
  if(!fSD1.Get())
  {
    G4String SD1name  = "GasSD";
    MFGasSD* aGasSD = new MFGasSD(SD1name,
                                  "GasHitsCollection");
    fSD1.Put(aGasSD);

    G4String SD2name  = "WatchSD";
    MFWatchSD* aWatchSD = new MFWatchSD(SD2name,
                                        "WatchHitsCollection");
    fSD2.Put(aWatchSD);

    // Also only add it once to the SD manager!
    G4SDManager::GetSDMpointer()->AddNewDetector(fSD1.Get());
    G4SDManager::GetSDMpointer()->AddNewDetector(fSD2.Get());

    SetSensitiveDetector("Gas_log", fSD1.Get());
    SetSensitiveDetector("Stop_log", fSD2.Get());
  }

  // Construct the field creator - this will register the field it creates

  if (!fEmFieldSetup.Get()) {
    MFMagneticFieldSetup* fieldSetup = new MFMagneticFieldSetup();
    G4AutoDelete::Register(fieldSetup); //Kernel will delete the messenger
    fEmFieldSetup.Put(fieldSetup);
  }
}

auto MFDetectorConstruction::SetupShort() -> G4VPhysicalVolume*
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

  fStepLimit = new G4UserLimits(maxStep);
  gasLogical->SetUserLimits(fStepLimit);

  return worldPhysical;
}

void MFDetectorConstruction::SetMaxStep(G4double s)
{
  maxStep = s * cm;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void MFDetectorConstruction::DefineCommands()
{
  // Define geometry command directory using generic messenger class
  fDetectorMessenger = new G4GenericMessenger(this, "/MF/detector/",
                                              "Commands for controlling detector setup");

  // switch command
  fDetectorMessenger->DeclareMethod("setMaxStep", &MFDetectorConstruction::SetMaxStep)
    .SetGuidance("Set maximum step length [cm]")
    .SetStates(G4State_PreInit)
    .SetToBeBroadcasted(false);
}
