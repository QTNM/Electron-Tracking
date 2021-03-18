#ifndef SEDetectorConstruction_h
#define SEDetectorConstruction_h 1

#include "G4Cache.hh"
#include "G4GenericMessenger.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4MultiFunctionalDetector;
class G4GlobalMagFieldMessenger;

class SEDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  SEDetectorConstruction();
  ~SEDetectorConstruction();

public:
  virtual G4VPhysicalVolume* Construct();
  virtual void               ConstructSDandField();

  void     SetGeometry(const G4String& name);

private:
  void DefineCommands();
  void DefineMaterials();

  G4VPhysicalVolume* SetupBaseline();
  G4VPhysicalVolume* SetupBunches();

  G4GenericMessenger*                       fDetectorMessenger = nullptr;
  G4String                                  fGeometryName      = "baseline";
  G4VectorCache<G4MultiFunctionalDetector*> fSD                = nullptr;
  G4Cache<G4GlobalMagFieldMessenger*>       fFieldMessenger    = nullptr;
};

#endif
