#ifndef MFRunAction_h
#define MFRunAction_h 1

#include "G4UserRunAction.hh"
#include "G4RootAnalysisManager.hh"
#include "globals.hh"

class G4Run;

/// Run action class
///

class MFRunAction : public G4UserRunAction
{
  using G4AnalysisManager = G4RootAnalysisManager;

public:
  MFRunAction(G4String name);
  virtual ~MFRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  G4String         fout;          // output file name
};


#endif
