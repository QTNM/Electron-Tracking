#ifndef CDDetectorConstruction_h
#define CDDetectorConstruction_h 1

#include "G4Cache.hh"
#include "G4GenericMessenger.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class CDGasSD;

class CDDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  CDDetectorConstruction();
  ~CDDetectorConstruction();

public:
  virtual G4VPhysicalVolume* Construct();
  virtual void               ConstructSDandField();

private:
  void DefineCommands();
  void DefineMaterials();

  G4VPhysicalVolume* SetupIsotrak();
  G4VPhysicalVolume* SetupQSA();
  void Switch(G4bool);
  
  G4GenericMessenger*                       fDetectorMessenger = nullptr;
  G4bool                                    fSource;
  G4Cache<CDGasSD*>                         fSD                = nullptr;
};

#endif
