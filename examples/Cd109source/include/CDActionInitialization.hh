#ifndef CDActionInitialization_h
#define CDActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "CDDetectorConstruction.hh"
#include "G4String.hh"

/// Action initialization class.

class CDActionInitialization : public G4VUserActionInitialization
{
public:
  CDActionInitialization(G4String name, CDDetectorConstruction* detector);
  virtual ~CDActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:
  G4String foutname;
  CDDetectorConstruction* _detector;
};


#endif
