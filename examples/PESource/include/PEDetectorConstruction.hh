#ifndef PEDetectorConstruction_h
#define PEDetectorConstruction_h 1

#include "G4Cache.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class PEGasSD;

class PEDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  PEDetectorConstruction();
  ~PEDetectorConstruction();

public:
  virtual G4VPhysicalVolume* Construct();
  virtual void               ConstructSDandField();

private:
  void DefineMaterials();

  G4VPhysicalVolume* Setup();
  G4Cache<PEGasSD*>  fSD = nullptr;
};

#endif
