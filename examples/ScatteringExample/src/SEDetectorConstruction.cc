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

#include "G4MultiFunctionalDetector.hh"
#include "G4SDManager.hh"
#include "G4SDParticleFilter.hh"
#include "G4VPrimitiveScorer.hh"
#include "SEPSKinEnergy.hh"
#include "SEPSBoundaryTime.hh"
#include "SEPSTime.hh"
#include "SEPSBoundaryTrackID.hh"
#include "SEPSTrackID.hh"
#include "SEPSParentID.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

SEDetectorConstruction::SEDetectorConstruction()
{
  DefineCommands();
  DefineMaterials();
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
  auto* gasMat = new G4Material("gas", 5.e-10 * g / cm3, 1);  // low density gas
  gasMat->AddElement(H3, 1);

  auto* bunchGas = new G4Material("bunch", 5.e-14 * g / cm3, 1);  // ultra-low density gas
  bunchGas->AddElement(H3, 1);
}

void SEDetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // Only need to construct the (per-thread) SD once
  if(!fSD.Size())
  {
    auto* gasdet = new G4MultiFunctionalDetector("Gas");
    fSD.Push_back(gasdet);

    auto* electronFilter = new G4SDParticleFilter("efilt");
    electronFilter->add("e-");  // register only electrons

    auto* primitive = new SEPSKinEnergy("KinE");
    primitive->SetFilter(electronFilter);
    gasdet->RegisterPrimitive(primitive);

    auto* tprimitive = new SEPSTime("Time");
    tprimitive->SetFilter(electronFilter);
    gasdet->RegisterPrimitive(tprimitive);

    auto* rprimitive = new SEPSTrackID("TrackID");
    rprimitive->SetFilter(electronFilter);
    gasdet->RegisterPrimitive(rprimitive);

    auto* pprimitive = new SEPSParentID("PID");
    pprimitive->SetFilter(electronFilter);
    gasdet->RegisterPrimitive(pprimitive);

    // Also only add it once to the SD manager!
    G4SDManager::GetSDMpointer()->AddNewDetector(fSD[0]);

    SetSensitiveDetector("Gas_log", fSD[0]);

    auto* stopdet = new G4MultiFunctionalDetector("Stop");
    fSD.Push_back(stopdet);

    auto* tbprimitive = new SEPSBoundaryTrackID("ExitID");
    tbprimitive->SetFilter(electronFilter);
    stopdet->RegisterPrimitive(tbprimitive);

    auto* ttprimitive = new SEPSBoundaryTime("ExitT");
    ttprimitive->SetFilter(electronFilter);
    stopdet->RegisterPrimitive(ttprimitive);

    G4SDManager::GetSDMpointer()->AddNewDetector(fSD[1]);
    SetSensitiveDetector("Stop_log", fSD[1]);
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
  G4double worldhZ    = 0.5 * m;    // box half side in z; full 1 metre
   
  // tubes with Gas ROI
  G4double pipewall  = 0.1 * cm;    // tube thickness 1 mm
  G4double piperad   = 2.0 * cm;    // tube diam 4 cm
  G4double pipehZ    = worldhZ - 1 * cm;  // fit into world 1cm either side
 
  // bunches of T-atoms
  G4double bunchhZ   = 5.0 * cm;    // 10cm long bunches in z  
  G4double bunchrad  = 1.0 * mm;    // small cylinders 1mm radius
  G4double gap       = 5.0 * cm;    // gap between bunches
  G4int nbunches     = (int) (pipehZ / (bunchhZ + gap)); // integer, lower limit half number
  
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
    new G4PVPlacement(nullptr, G4ThreeVector(0. * cm, 0. * cm, -pipehZ + (i+1)*(bunchhZ+gap)), gasLogical,
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
  G4double bunchrad  = 1.0 * mm;    // small cylinders 1mm radius
  G4double gap       = 5.0 * cm;    // gap between bunches
  G4int nbunches     = (int) (pipehZ / (bunchhZ + gap)); // integer, lower limit half number
 
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
    new G4PVPlacement(nullptr, G4ThreeVector(0. * cm, 0. * cm, -pipehZ + (i+1)*(bunchhZ+gap)), gasLogical,
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

}
