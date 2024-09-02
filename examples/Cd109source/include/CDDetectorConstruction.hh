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
  virtual G4VPhysicalVolume* Construct() override;
  virtual void               ConstructSDandField() override;
  G4String                   DetectorType();

  G4double _r_min;
  G4double _r_max;

private:
  void DefineCommands();
  void DefineMaterials();

  G4VPhysicalVolume* SetupIsotrak();
  G4VPhysicalVolume* SetupQSA();
  G4VPhysicalVolume* SetupPointlike();
  G4VPhysicalVolume* SetupShell();
  void Switch(G4String);

  G4GenericMessenger*                       fDetectorMessenger = nullptr;
  G4String                                  fSource;
  G4Cache<CDGasSD*>                         fSD                = nullptr;

};

#endif
