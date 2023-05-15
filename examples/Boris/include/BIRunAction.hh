#ifndef BIRunAction_h
#define BIRunAction_h 1

#include "G4UserRunAction.hh"
#include "G4RootAnalysisManager.hh"
#include "globals.hh"

class G4Run;

/// Run action class
///

class BIRunAction : public G4UserRunAction
{
  using G4AnalysisManager = G4RootAnalysisManager;

public:
  BIRunAction(G4String name);
  virtual ~BIRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  G4String         fout;          // output file name
};


#endif
