#ifndef CDRunAction_h
#define CDRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

/// Run action class
///

class CDRunAction : public G4UserRunAction
{
public:
  CDRunAction(G4String name);
  virtual ~CDRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  G4String         fout;          // output file name
};


#endif
