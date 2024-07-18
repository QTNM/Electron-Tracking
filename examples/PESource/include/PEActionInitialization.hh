#ifndef PEActionInitialization_h
#define PEActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "G4String.hh"

/// Action initialization class.

class PEActionInitialization : public G4VUserActionInitialization
{
public:
  PEActionInitialization(G4String name);
  virtual ~PEActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:
  G4String foutname;
};

#endif
