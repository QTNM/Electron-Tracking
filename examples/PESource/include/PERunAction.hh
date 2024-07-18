#ifndef PERunAction_h
#define PERunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

/// Run action class
///

class PERunAction : public G4UserRunAction
{
public:
  PERunAction(G4String name);
  virtual ~PERunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  G4String         fout;          // output file name
};


#endif
