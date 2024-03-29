#include "SEDetectorConstruction.hh"

#include <set>

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
#include "SEGasSD.hh"
#include "SEWatchSD.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

SEDetectorConstruction::SEDetectorConstruction()
{
  fdensity = 5.e-12 * g / cm3;

  DefineCommands();
}

SEDetectorConstruction::~SEDetectorConstruction()
{
  delete fDetectorMessenger;
}

auto SEDetectorConstruction::Construct() -> G4VPhysicalVolume*
{
  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  DefineMaterials();

  if(fGeometryName == "bunches")
  {
    return SetupBunches();
  }
  else if (fGeometryName == "shortPipe")
  {
    return SetupShort();
  }

  return SetupBaseline();
}

void SEDetectorConstruction::DefineMaterials()
{
  G4NistManager* nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Galactic");
  nistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL");

  auto* H3     = new G4Element("Tritium", "H", 1., 3.016 * g / mole);
  // auto* gasMat = new G4Material("gas", 5.e-10 * g / cm3, 1);  // low density gas
  auto* gasMat = new G4Material("gas", fdensity, 1);  // low density gas
  gasMat->AddElement(H3, 1);

  auto* bunchGas = new G4Material("bunch", fdensity, 1);  // ultra-low density gas
  bunchGas->AddElement(H3, 1);
}

void SEDetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // Only need to construct the (per-thread) SD once
  if(!fSD1.Get()) // both declared together, test one is enough
  {
    G4String SD1name  = "GasSD";
    SEGasSD* aGasSD = new SEGasSD(SD1name,
                                  "GasHitsCollection");
    fSD1.Put(aGasSD);

    G4String SD2name  = "WatchSD";
    SEWatchSD* aWatchSD = new SEWatchSD(SD2name,
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

auto SEDetectorConstruction::SetupShort() -> G4VPhysicalVolume*
{
  // Get materials
  auto* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  auto* steelMat      = G4Material::GetMaterial("G4_STAINLESS-STEEL");
  auto* bunchMat      = G4Material::GetMaterial("bunch");
    
  // size parameter, unit [cm]
  // world
  G4double worldhside = 5.0 * cm;   // box half side in x, y
  G4double worldhZ    = 0.51 * m;    // box half side in z; full 1.02 metre
   
  // tubes with Gas ROI
  G4double pipewall  = 0.1 * cm;    // tube thickness 1 mm
  G4double piperad   = 2.0 * cm;    // tube diam 4 cm
  G4double pipehZ    = worldhZ - 1 * cm;  // fit into world 1cm either side
 
  // bunches of T-atoms
  G4double bunchhZ   = 5.0 * cm;    // 10 cm long bunches in z  
  G4double bunchrad  = 2.0 * mm;    // small cylinders 4 mm diam
  G4double gap       = 2.0 * cm;    // gap between bunches
  G4int nbunches     = (int) (pipehZ / (2*bunchhZ + gap)); // integer, lower limit half number

  G4cout << ">> shortPipe: nbunches fit in pipe: " << 2*nbunches-2 << G4endl;
  G4cout << ">> shortPipe: at density [g/cm3] " << bunchMat->GetDensity() / (g / cm3) << G4endl;
  
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
  auto* gasSolid = new G4Tubs("Gas", 0.0 * cm, bunchrad, bunchhZ,
                               0.0, CLHEP::twopi);
  
  //
  // stopwatch disk at end of pipe
  auto* stopSolid = new G4Tubs("Stop", 0.0 * cm, piperad, heightZ,
                                0.0, CLHEP::twopi);
  
  // logical volumes
  auto* pipeLogical = new G4LogicalVolume(pipeSolid, worldMaterial, "Pipe_log");
  auto* gasLogical  = new G4LogicalVolume(gasSolid, bunchMat, "Gas_log");
  auto* stopLogical = new G4LogicalVolume(stopSolid, worldMaterial, "Stop_log");
    
  // placements
  new G4PVPlacement(nullptr, G4ThreeVector(0. * cm, 0. * cm, 0. * cm),
                    pipeLogical, "Pipe_phys", worldLogical, false, 0, true);
  
  new G4PVPlacement(nullptr, G4ThreeVector(0. * cm, 0. * cm, pipehZ + heightZ), stopLogical,
                    "Stop_phys", worldLogical, false, 0, true);
  
  for (G4int i=0; i<2*nbunches-1; ++i)
    new G4PVPlacement(nullptr, G4ThreeVector(0. * cm, 0. * cm, -pipehZ + (i+1)*(2*bunchhZ+gap)), gasLogical,
                      "Bunch_phys", worldLogical, false, i, true); // each a copy number
    
  return worldPhysical;
}


auto SEDetectorConstruction::SetupBunches() -> G4VPhysicalVolume*
{
  // Get materials
  auto* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  auto* steelMat      = G4Material::GetMaterial("G4_STAINLESS-STEEL");
  auto* bunchMat      = G4Material::GetMaterial("bunch");

  // size parameter, unit [cm]
  // world
  G4double worldhside = 5.0 * cm;   // box half side in x, y
  G4double worldhZ    = 39.5 * km;  // box half side in z; kilometer-long

  // tubes with Gas ROI
  G4double pipewall  = 0.1 * cm;    // tube thickness 1 mm
  G4double piperad   = 2.0 * cm;    // tube diam 4 cm
  G4double pipehZ    = worldhZ - 1 * cm;  // fit into world 1cm either side

  // bunches of T-atoms
  G4double bunchhZ   = 5.0 * cm;    // 10cm long bunches in z
  G4double bunchrad  = 2.0 * mm;    // small cylinders 4 mm diam
  G4double gap       = 5.0 * cm;    // gap between bunches
  G4int nbunches     = (int) (pipehZ / (2*bunchhZ + gap)); // integer, lower limit half number
 
  G4cout << ">> bunches: nbunches fit in pipe: " << 2*nbunches-2 << G4endl;

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
  auto* gasSolid = new G4Tubs("Gas", 0.0 * cm, bunchrad, bunchhZ,
                               0.0, CLHEP::twopi);

  //
  // stopwatch disk at end of pipe
  auto* stopSolid = new G4Tubs("Stop", 0.0 * cm, piperad, heightZ,
                                0.0, CLHEP::twopi);

  // logical volumes
  auto* pipeLogical = new G4LogicalVolume(pipeSolid, worldMaterial, "Pipe_log");
  auto* gasLogical  = new G4LogicalVolume(gasSolid, bunchMat, "Gas_log");
  auto* stopLogical = new G4LogicalVolume(stopSolid, worldMaterial, "Stop_log");

  // placements
  new G4PVPlacement(nullptr, G4ThreeVector(0. * cm, 0. * cm, 0. * cm),
                    pipeLogical, "Pipe_phys", worldLogical, false, 0, true);

  new G4PVPlacement(nullptr, G4ThreeVector(0. * cm, 0. * cm, pipehZ + heightZ), stopLogical,
                    "Stop_phys", worldLogical, false, 0, true);

  for (G4int i=0; i<2*nbunches-1; ++i) 
    new G4PVPlacement(nullptr, G4ThreeVector(0. * cm, 0. * cm, -pipehZ + (i+1)*(2*bunchhZ+gap)), gasLogical,
                      "Bunch_phys", worldLogical, false, i, false); // each a copy number



  return worldPhysical;
}

auto SEDetectorConstruction::SetupBaseline() -> G4VPhysicalVolume*
{
  // Get materials
  auto* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  auto* steelMat      = G4Material::GetMaterial("G4_STAINLESS-STEEL");
  auto* gasMat        = G4Material::GetMaterial("gas");
    
  // size parameter, unit [cm]
  // world
  G4double worldhside = 5.0 * cm;   // box half side in x, y
  G4double worldhZ    = 39.5 * km;  // box half side in z; kilometer-long
  
  // tubes with Gas ROI
  G4double pipewall  = 0.1 * cm;    // tube thickness 1 mm
  G4double piperad   = 2.0 * cm;    // tube diam 4 cm
  G4double pipehZ    = worldhZ - 1*cm;  // fit into world 1cm either side
    
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
                    
  //
  // stopwatch disk at end of pipe
  auto* stopSolid = new G4Tubs("Stop", 0.0 * cm, piperad, heightZ,
                                0.0, CLHEP::twopi);
  
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



void SEDetectorConstruction::SetGeometry(const G4String& name)
{
  std::set<G4String> knownGeometries = { "baseline", "bunches", "shortPipe" };
  if(knownGeometries.count(name) == 0)
  {
    G4Exception("SEDetectorConstruction::SetGeometry", "SE0001", JustWarning,
                ("Invalid geometry setup name '" + name + "'").c_str());
    return;
  }

  fGeometryName = name;
  // Reinit wiping out stores
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void SEDetectorConstruction::SetDensity(G4double d)
{
  if(d <= 0.)
  {
    G4Exception("SEDetectorConstruction::SetDensity", "SE0001", JustWarning,
                "Invalid density value ");
    return;
  }
  
  fdensity = d * g/cm3;
  // Reinit wiping out stores
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}   

void SEDetectorConstruction::DefineCommands()
{
  // Define geometry command directory using generic messenger class
  fDetectorMessenger = new G4GenericMessenger(this, "/SE/detector/",
                                              "Commands for controlling detector setup");

  // switch command
  fDetectorMessenger->DeclareMethod("setGeometry", &SEDetectorConstruction::SetGeometry)
    .SetGuidance("Set geometry model of cavern and detector")
    .SetGuidance("baseline = NEEDS DESCRIPTION")
    .SetGuidance("bunches = NEEDS DESCRIPTION")
    .SetGuidance("shortPipe = NEEDS DESCRIPTION")
    .SetCandidates("baseline bunches shortPipe")
    .SetStates(G4State_PreInit)
    .SetToBeBroadcasted(false);

  fDetectorMessenger->DeclareMethod("setDensity", &SEDetectorConstruction::SetDensity)
    .SetGuidance("Set detector T-gas density [g/cm^3]")
    .SetStates(G4State_PreInit)
    .SetToBeBroadcasted(false);

}
