#ifndef EGRunAction_h
#define EGRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

/// Run action class
///

class EGRunAction : public G4UserRunAction
{
public:
  EGRunAction(G4String name);
  virtual ~EGRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  G4String         fout;          // output file name
};


#endif
