#ifndef SEDetectorConstruction_h
#define SEDetectorConstruction_h 1

#include "G4Cache.hh"
#include "G4GenericMessenger.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;
class SEGasSD;
class SEWatchSD;

class SEDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  SEDetectorConstruction();
  ~SEDetectorConstruction();

public:
  virtual G4VPhysicalVolume* Construct();
  virtual void               ConstructSDandField();

  void     SetGeometry(const G4String& name);
  void     SetDensity(G4double d);

private:
  void DefineCommands();
  void DefineMaterials();

  G4VPhysicalVolume* SetupBaseline();
  G4VPhysicalVolume* SetupBunches();
  G4VPhysicalVolume* SetupShort();

  G4GenericMessenger*                       fDetectorMessenger = nullptr;
  G4String                                  fGeometryName      = "baseline";
  G4double                                  fdensity;
  G4Cache<G4GlobalMagFieldMessenger*>       fFieldMessenger    = nullptr;
  G4Cache<SEGasSD*>                         fSD1               = nullptr;
  G4Cache<SEWatchSD*>                       fSD2               = nullptr;
};

#endif
