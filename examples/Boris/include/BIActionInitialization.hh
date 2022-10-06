#ifndef BIActionInitialization_h
#define BIActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "G4String.hh"

/// Action initialization class.

class BIActionInitialization : public G4VUserActionInitialization
{
public:
  BIActionInitialization(G4String name);
  virtual ~BIActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:
  G4String foutname;
};

#endif
