#ifndef MFDetectorConstruction_h
#define MFDetectorConstruction_h 1

#include "G4Cache.hh"
#include "G4GenericMessenger.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4UserLimits.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class MFGasSD;
class MFWatchSD;
class G4GlobalMagFieldMessenger;
class MFMagneticFieldSetup;

class MFDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  MFDetectorConstruction();
  ~MFDetectorConstruction();

public:
  virtual G4VPhysicalVolume* Construct();
  virtual void               ConstructSDandField();

  void     SetMaxStep(G4double s);

private:
  void DefineCommands();
  void DefineMaterials();

  G4VPhysicalVolume* SetupShort();

  G4GenericMessenger*                       fDetectorMessenger = nullptr;
  G4Cache<MFGasSD*>                         fSD1               = nullptr;
  G4Cache<MFWatchSD*>                       fSD2               = nullptr;
  G4Cache<MFMagneticFieldSetup*>            fEmFieldSetup      = nullptr;
  G4UserLimits*                             fStepLimit         = nullptr;
  G4double                                  maxStep;
};

#endif
