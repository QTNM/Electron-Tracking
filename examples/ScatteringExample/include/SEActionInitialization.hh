#ifndef SEActionInitialization_h
#define SEActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "G4String.hh"

/// Action initialization class.

class SEActionInitialization : public G4VUserActionInitialization
{
public:
  SEActionInitialization(G4String name);
  virtual ~SEActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:
  G4String foutname;
};

#endif
