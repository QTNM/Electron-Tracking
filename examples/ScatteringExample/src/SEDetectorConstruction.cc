#include "SEDetectorConstruction.hh"

#include <cmath>
#include <set>

#include "G4RunManager.hh"

#include "G4Box.hh"
#include "G4GeometryManager.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4Tubs.hh"

#include "G4GlobalMagFieldMessenger.hh"
#include "G4UniformMagField.hh"
#include "G4AutoDelete.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4SDManager.hh"
#include "G4SDParticleFilter.hh"
#include "G4VPrimitiveScorer.hh"
#include "SEPSKinEnergy.hh"
#include "SEPSTime.hh"
#include "SEPSTrackID.hh"

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

  if(fGeometryName == "baseline")
  {
    return SetupBaseline();
  }

  return SetupBunches();
}

void SEDetectorConstruction::DefineMaterials()
{
  G4NistManager* nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Galactic");
  nistManager->FindOrBuildMaterial("G4_lAr");

  auto* H      = new G4Element("Hydrogen", "H", 1., 1.00794 * g / mole);
  auto* gasMat = new G4Material("gas", 0.3 * g / cm3, 1);  // low density gas
  gasMat->AddElement(H, 1);

}

void SEDetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(2);

  // Only need to construct the (per-thread) SD once
  if(!fSD.Get())
  {
    auto* det = new G4MultiFunctionalDetector("Det");
    fSD.Put(det);

    auto* vertexFilter = new G4SDParticleFilter("vtxfilt");
    vertexFilter->addIon(32, 77);  // register 77Ge production

    auto* eprimitive = new SEPSKinEnergy("KinE", 1);
    eprimitive->SetFilter(vertexFilter);
    det->RegisterPrimitive(eprimitive);


    // Also only add it once to the SD manager!
    G4SDManager::GetSDMpointer()->AddNewDetector(fSD.Get());

    SetSensitiveDetector("Ge_log", fSD.Get());
  }
  else
  {
    G4cout << " >>> fSD has entry. Repeated call." << G4endl;
  }
}

auto SEDetectorConstruction::SetupBunches() -> G4VPhysicalVolume*
{
  // Get materials
  auto* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  auto* larMat        = G4Material::GetMaterial("G4_lAr");

  // size parameter, unit [cm]
  // cavern
  G4double stone     = 100.0;  // Hall wall thickness 1 m
  G4double hallhside = 850.0;  // Hall cube side 17 m
  // cryostat
  G4double tankhside  = 650;   // cryostat cube side 13 m
  G4double outerwall  = 1.2;   // outer SS wall thickness
  G4double insulation = 80.0;  // polyurethane foam
  G4double innerwall  = 0.12;  // inner SS membrane
  // copper tubes with Germanium ROI
  G4double copper    = 0.35;    // tube thickness 3.5 mm
  G4double curad     = 40.0;    // copper tube diam 80 cm
  G4double cuhheight = 334.34;  // copper tube height 7 m inside cryostat
  G4double cushift   = 234.34;  // shift cu tube inside cryostat to top
  G4double ringrad   = 100.0;   // cu tube placement ring radius
  // Ge cylinder for 250 kg at 5.32 g/cm3
  G4double roiradius     = 25.0;   // detector region diam 50 cm
  G4double roihalfheight = 11.97;  // detector region height 24 cm

  // total
  G4double offset =
    hallhside - tankhside;  // shift cavern floor to keep detector centre at origin
  G4double worldside = hallhside + stone + offset + 0.1;  // larger than rest
  G4double larside =
    tankhside - outerwall - insulation - innerwall;  // cube side of LAr volume

  fvertexZ = (worldside - stone - 0.1) * cm;  // max vertex height
  fmaxrad  = 1000.0 * cm;                   // max vertex circle radius

  // Volumes for this geometry

  //
  // World
  //
  auto* worldSolid = new G4Box("World", worldside * cm, worldside * cm, worldside * cm);
  auto* fWorldLogical  = new G4LogicalVolume(worldSolid, worldMaterial, "World_log");
  auto* fWorldPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fWorldLogical,
                                           "World_phys", nullptr, false, 0);

  //
  // Cavern
  //
  auto* cavernSolid    = new G4Box("Cavern", (hallhside + stone) * cm,
                                (hallhside + stone) * cm, (hallhside + stone) * cm);
  auto* fCavernLogical = new G4LogicalVolume(cavernSolid, stdRock, "Cavern_log");
  auto* fCavernPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., offset * cm), fCavernLogical,
                      "Cavern_phys", fWorldLogical, false, 0);

  //
  // Hall
  //
  auto* hallSolid = new G4Box("Cavern", hallhside * cm, hallhside * cm, hallhside * cm);
  auto* fHallLogical = new G4LogicalVolume(hallSolid, airMat, "Hall_log");
  auto* fHallPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -stone * cm), fHallLogical,
                      "Hall_phys", fCavernLogical, false, 0, true);

  //
  // Tank
  //
  auto* tankSolid    = new G4Box("Tank", tankhside * cm, tankhside * cm, tankhside * cm);
  auto* fTankLogical = new G4LogicalVolume(tankSolid, steelMat, "Tank_log");
  auto* fTankPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -stone * cm), fTankLogical,
                      "Tank_phys", fHallLogical, false, 0, true);

  //
  // Insulator
  //
  auto* puSolid     = new G4Box("Insulator", (tankhside - outerwall) * cm,
                            (tankhside - outerwall) * cm, (tankhside - outerwall) * cm);
  auto* fPuLogical  = new G4LogicalVolume(puSolid, puMat, "Pu_log");
  auto* fPuPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fPuLogical, "Pu_phys",
                                        fTankLogical, false, 0, true);

  //
  // Membrane
  //
  auto* membraneSolid = new G4Box("Membrane", (tankhside - outerwall - insulation) * cm,
                                  (tankhside - outerwall - insulation) * cm,
                                  (tankhside - outerwall - insulation) * cm);
  auto* fMembraneLogical = new G4LogicalVolume(membraneSolid, steelMat, "Membrane_log");
  auto* fMembranePhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(), fMembraneLogical, "Membrane_phys",
                      fPuLogical, false, 0, true);

  //
  // LAr
  //
  auto* larSolid     = new G4Box("LAr", larside * cm, larside * cm, larside * cm);
  auto* fLarLogical  = new G4LogicalVolume(larSolid, larMat, "Lar_log");
  auto* fLarPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fLarLogical,
                                         "Lar_phys", fMembraneLogical, false, 0, true);

  //
  // copper tubes, hollow cylinder shell
  //
  auto* copperSolid = new G4Tubs("Copper", (curad - copper) * cm, curad * cm,
                                 cuhheight * cm, 0.0, CLHEP::twopi);

  //
  // ULAr bath, solid cylinder
  //
  auto* ularSolid = new G4Tubs("ULar", 0.0 * cm, (curad - copper) * cm, cuhheight * cm,
                               0.0, CLHEP::twopi);

  //
  // Germanium, solid cylinder
  //
  auto* geSolid =
    new G4Tubs("ROI", 0.0 * cm, roiradius * cm, roihalfheight * cm, 0.0, CLHEP::twopi);

  // tower; logical volumes
  auto* fCopperLogical = new G4LogicalVolume(copperSolid, copperMat, "Copper_log");
  auto* fUlarLogical   = new G4LogicalVolume(ularSolid, larMat, "ULar_log");
  auto* fGeLogical     = new G4LogicalVolume(geSolid, roiMat, "Ge_log");

  // placements
  new G4PVPlacement(nullptr, G4ThreeVector(ringrad * cm, 0., cushift * cm),
                    fCopperLogical, "Copper_phys", fLarLogical, false, 0, true);

  new G4PVPlacement(nullptr, G4ThreeVector(ringrad * cm, 0., cushift * cm), fUlarLogical,
                    "ULar_phys", fLarLogical, false, 0, true);

  new G4PVPlacement(nullptr, G4ThreeVector(0. * cm, 0. * cm, -cushift * cm), fGeLogical,
                    "Ge_phys", fUlarLogical, false, 0, true);

  // tower 2
  new G4PVPlacement(nullptr, G4ThreeVector(0., ringrad * cm, cushift * cm),
                    fCopperLogical, "Copper_phys2", fLarLogical, false, 1, true);

  new G4PVPlacement(nullptr, G4ThreeVector(0., ringrad * cm, cushift * cm), fUlarLogical,
                    "ULar_phys2", fLarLogical, false, 1, true);

  // tower 3
  new G4PVPlacement(nullptr, G4ThreeVector(-ringrad * cm, 0., cushift * cm),
                    fCopperLogical, "Copper_phys3", fLarLogical, false, 2, true);

  new G4PVPlacement(nullptr, G4ThreeVector(-ringrad * cm, 0., cushift * cm), fUlarLogical,
                    "ULar_phys3", fLarLogical, false, 2, true);

  // tower 4
  new G4PVPlacement(nullptr, G4ThreeVector(0., -ringrad * cm, cushift * cm),
                    fCopperLogical, "Copper_phys4", fLarLogical, false, 3, true);

  new G4PVPlacement(nullptr, G4ThreeVector(0., -ringrad * cm, cushift * cm), fUlarLogical,
                    "ULar_phys4", fLarLogical, false, 3, true);


  return fWorldPhysical;
}

auto SEDetectorConstruction::SetupBaseline() -> G4VPhysicalVolume*
{
  // Get materials
  auto* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  auto* larMat        = G4Material::GetMaterial("G4_lAr");

  // size parameter, unit [cm]
  G4double offset = 200.0;  // shift cavern floor to keep detector centre at origin
  // cavern
  G4double stone       = 100.0;  // Hall wall thickness 1 m
  G4double hallrad     = 600.0;  // Hall cylinder diam 12 m
  G4double hallhheight = 850.0;  // Hall cylinder height 17 m
  // water tank
  G4double tankwalltop = 0.6;  // water tank thickness at top 6 mm
  G4double tankwallbot = 0.8;  // water tank thickness at bottom 8 mm
  G4double tankrad     = 550;  // water tank diam 11 m
  G4double tankhheight = 650;  // water tank height 13 m
  // cryostat
  G4double cryowall   = 3.0;    // cryostat wall thickness from GERDA
  G4double vacgap     = 1.0;    // vacuum gap between walls
  G4double cryrad     = 350.0;  // cryostat diam 7 m
  G4double cryhheight = 350.0;  // cryostat height 7 m
  // copper tubes with Germanium ROI
  G4double copper    = 0.35;   // tube thickness 3.5 mm
  G4double curad     = 40.0;   // copper tube diam 80 cm
  G4double cuhheight = 200.0;  // copper tube height 4 m inside cryostat
  G4double cushift   = 150.0;  // shift cu tube inside cryostat to top
  G4double ringrad   = 100.0;  // cu tube placement ring radius
  // Ge cylinder for 250 kg at 5.32 g/cm3
  G4double roiradius     = 25.0;   // detector region diam 50 cm
  G4double roihalfheight = 11.97;  // detector region height 24 cm

  fvertexZ = (hallhheight + offset) * cm;
  fmaxrad  = (hallrad + stone) * cm;

  // Volumes for this geometry

  //
  // World
  //
  auto* worldSolid =
    new G4Tubs("World", 0.0 * cm, (hallrad + stone + 0.1) * cm,
               (hallhheight + stone + offset + 0.1) * cm, 0.0, CLHEP::twopi);
  auto* fWorldLogical  = new G4LogicalVolume(worldSolid, worldMaterial, "World_log");
  auto* fWorldPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fWorldLogical,
                                           "World_phys", nullptr, false, 0);

  //
  // Cavern
  //
  auto* cavernSolid    = new G4Tubs("Cavern", 0.0 * cm, (hallrad + stone) * cm,
                                 (hallhheight + stone) * cm, 0.0, CLHEP::twopi);
  auto* fCavernLogical = new G4LogicalVolume(cavernSolid, stdRock, "Cavern_log");
  auto* fCavernPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., offset * cm), fCavernLogical,
                      "Cavern_phys", fWorldLogical, false, 0);

  //
  // Hall
  //
  auto* hallSolid =
    new G4Tubs("Hall", 0.0 * cm, hallrad * cm, hallhheight * cm, 0.0, CLHEP::twopi);
  auto* fHallLogical = new G4LogicalVolume(hallSolid, airMat, "Hall_log");
  auto* fHallPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -stone * cm), fHallLogical,
                      "Hall_phys", fCavernLogical, false, 0, true);

  //
  // Tank
  //
  auto* tankSolid =
    new G4Cons("Tank", 0.0 * cm, (tankrad + tankwallbot) * cm, 0.0 * cm,
               (tankrad + tankwalltop) * cm, tankhheight * cm, 0.0, CLHEP::twopi);
  auto* fTankLogical = new G4LogicalVolume(tankSolid, steelMat, "Tank_log");
  auto* fTankPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -stone * cm), fTankLogical,
                      "Tank_phys", fHallLogical, false, 0, true);

  //
  // Water
  //
  auto* waterSolid     = new G4Tubs("Water", 0.0 * cm, tankrad * cm,
                                (tankhheight - tankwallbot) * cm, 0.0, CLHEP::twopi);
  auto* fWaterLogical  = new G4LogicalVolume(waterSolid, waterMat, "Water_log");
  auto* fWaterPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fWaterLogical,
                                           "Water_phys", fTankLogical, false, 0, true);

  //
  // outer cryostat
  //
  auto* coutSolid =
    new G4Tubs("Cout", 0.0 * cm, cryrad * cm, cryhheight * cm, 0.0, CLHEP::twopi);
  auto* fCoutLogical  = new G4LogicalVolume(coutSolid, steelMat, "Cout_log");
  auto* fCoutPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fCoutLogical,
                                          "Cout_phys", fWaterLogical, false, 0, true);

  //
  // vacuum gap
  //
  auto* cvacSolid     = new G4Tubs("Cvac", 0.0 * cm, (cryrad - cryowall) * cm,
                               cryhheight * cm, 0.0, CLHEP::twopi);
  auto* fCvacLogical  = new G4LogicalVolume(cvacSolid, worldMaterial, "Cvac_log");
  auto* fCvacPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fCvacLogical,
                                          "Cvac_phys", fCoutLogical, false, 0, true);

  //
  // inner cryostat
  //
  auto* cinnSolid     = new G4Tubs("Cinn", 0.0 * cm, (cryrad - cryowall - vacgap) * cm,
                               cryhheight * cm, 0.0, CLHEP::twopi);
  auto* fCinnLogical  = new G4LogicalVolume(cinnSolid, steelMat, "Cinn_log");
  auto* fCinnPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fCinnLogical,
                                          "Cinn_phys", fCvacLogical, false, 0, true);

  //
  // LAr bath
  //
  auto* larSolid     = new G4Tubs("LAr", 0.0 * cm, (cryrad - 2 * cryowall - vacgap) * cm,
                              cryhheight * cm, 0.0, CLHEP::twopi);
  auto* fLarLogical  = new G4LogicalVolume(larSolid, larMat, "Lar_log");
  auto* fLarPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fLarLogical,
                                         "Lar_phys", fCinnLogical, false, 0, true);

  //
  // cryostat Lid
  //
  auto* lidSolid =
    new G4Tubs("Lid", 0.0 * cm, cryrad * cm, cryowall / 2.0 * cm, 0.0, CLHEP::twopi);
  auto* fLidLogical = new G4LogicalVolume(lidSolid, steelMat, "Lid_log");
  auto* fLidPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., (cryhheight + cryowall / 2.0) * cm),
                      fLidLogical, "Lid_phys", fWaterLogical, false, 0, true);
  auto* fBotLogical = new G4LogicalVolume(lidSolid, steelMat, "Bot_log");
  auto* fBotPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -(cryhheight + cryowall / 2.0) * cm),
                      fBotLogical, "Bot_phys", fWaterLogical, false, 0, true);

  //
  // copper tubes, hollow cylinder shell
  //
  auto* copperSolid = new G4Tubs("Copper", (curad - copper) * cm, curad * cm,
                                 cuhheight * cm, 0.0, CLHEP::twopi);

  //
  // ULAr bath, solid cylinder
  //
  auto* ularSolid = new G4Tubs("ULar", 0.0 * cm, (curad - copper) * cm, cuhheight * cm,
                               0.0, CLHEP::twopi);

  //
  // Germanium, solid cylinder
  //
  auto* geSolid =
    new G4Tubs("ROI", 0.0 * cm, roiradius * cm, roihalfheight * cm, 0.0, CLHEP::twopi);

  // tower; logical volumes
  auto* fCopperLogical = new G4LogicalVolume(copperSolid, copperMat, "Copper_log");
  auto* fUlarLogical   = new G4LogicalVolume(ularSolid, larMat, "ULar_log");
  auto* fGeLogical     = new G4LogicalVolume(geSolid, roiMat, "Ge_log");

  // placements
  new G4PVPlacement(nullptr, G4ThreeVector(ringrad * cm, 0., cushift * cm),
                    fCopperLogical, "Copper_phys", fLarLogical, false, 0, true);

  new G4PVPlacement(nullptr, G4ThreeVector(ringrad * cm, 0., cushift * cm), fUlarLogical,
                    "ULar_phys", fLarLogical, false, 0, true);

  new G4PVPlacement(nullptr, G4ThreeVector(0. * cm, 0. * cm, -cushift * cm), fGeLogical,
                    "Ge_phys", fUlarLogical, false, 0, true);

  // tower 2
  new G4PVPlacement(nullptr, G4ThreeVector(0., ringrad * cm, cushift * cm),
                    fCopperLogical, "Copper_phys2", fLarLogical, false, 1, true);

  new G4PVPlacement(nullptr, G4ThreeVector(0., ringrad * cm, cushift * cm), fUlarLogical,
                    "ULar_phys2", fLarLogical, false, 1, true);

  // tower 3
  new G4PVPlacement(nullptr, G4ThreeVector(-ringrad * cm, 0., cushift * cm),
                    fCopperLogical, "Copper_phys3", fLarLogical, false, 2, true);

  new G4PVPlacement(nullptr, G4ThreeVector(-ringrad * cm, 0., cushift * cm), fUlarLogical,
                    "ULar_phys3", fLarLogical, false, 2, true);

  // tower 4
  new G4PVPlacement(nullptr, G4ThreeVector(0., -ringrad * cm, cushift * cm),
                    fCopperLogical, "Copper_phys4", fLarLogical, false, 3, true);

  new G4PVPlacement(nullptr, G4ThreeVector(0., -ringrad * cm, cushift * cm), fUlarLogical,
                    "ULar_phys4", fLarLogical, false, 3, true);


  return fWorldPhysical;
}



void SEDetectorConstruction::SetGeometry(const G4String& name)
{
  std::set<G4String> knownGeometries = { "baseline", "bunches" };
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
    .SetCandidates("baseline bunches ")
    .SetStates(G4State_PreInit)
    .SetToBeBroadcasted(false);

}
