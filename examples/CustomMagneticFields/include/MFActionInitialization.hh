#ifndef MFActionInitialization_h
#define MFActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "G4String.hh"

/// Action initialization class.

class MFActionInitialization : public G4VUserActionInitialization
{
public:
  MFActionInitialization(G4String name);
  virtual ~MFActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:
  G4String foutname;
};

#endif
