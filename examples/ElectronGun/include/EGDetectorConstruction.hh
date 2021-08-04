#ifndef EGDetectorConstruction_h
#define EGDetectorConstruction_h 1

#include "G4Cache.hh"
#include "G4GenericMessenger.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class EGGasSD;

class EGDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  EGDetectorConstruction();
  ~EGDetectorConstruction();

public:
  virtual G4VPhysicalVolume* Construct();
  virtual void               ConstructSDandField();

  void     SetDensity(G4double d);

private:
  void DefineCommands();
  void DefineMaterials();

  G4VPhysicalVolume* SetupShort();

  G4GenericMessenger*                       fDetectorMessenger = nullptr;
  G4double                                  fdensity;
  G4Cache<EGGasSD*>                         fSD                = nullptr;
};

#endif
