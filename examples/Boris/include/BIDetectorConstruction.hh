#ifndef BIDetectorConstruction_h
#define BIDetectorConstruction_h 1

#include "G4Cache.hh"
#include "G4GenericMessenger.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class BIGasSD;
class BIWatchSD;
class G4GlobalMagFieldMessenger;
class BIMagneticFieldSetup;

class BIDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  BIDetectorConstruction();
  ~BIDetectorConstruction();

public:
  virtual G4VPhysicalVolume* Construct();
  virtual void               ConstructSDandField();

private:
  void DefineCommands();
  void DefineMaterials();

  G4VPhysicalVolume* SetupShort();

  G4GenericMessenger*                       fDetectorMessenger = nullptr;
  G4Cache<BIGasSD*>                         fSD1               = nullptr;
  G4Cache<BIWatchSD*>                       fSD2               = nullptr;
  G4Cache<BIMagneticFieldSetup*>            fEmFieldSetup      = nullptr;
};

#endif
