#ifndef EGDetectorConstruction_h
#define EGDetectorConstruction_h 1

#include "G4Cache.hh"
#include "G4GenericMessenger.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class EGGasSD;
class EGWatchSD;

class EGDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  EGDetectorConstruction();
  ~EGDetectorConstruction();

public:
  virtual G4VPhysicalVolume* Construct();
  virtual void               ConstructSDandField();

private:
  void DefineCommands();
  void DefineMaterials();

  G4VPhysicalVolume* SetupShort();

  G4GenericMessenger*                       fDetectorMessenger = nullptr;
  G4Cache<EGGasSD*>                         fSD1               = nullptr;
  G4Cache<EGWatchSD*>                       fSD2               = nullptr;
};

#endif
