#ifndef EGActionInitialization_h
#define EGActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "G4String.hh"

/// Action initialization class.

class EGActionInitialization : public G4VUserActionInitialization
{
public:
  EGActionInitialization(G4String name);
  virtual ~EGActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:
  G4String foutname;
};

#endif
