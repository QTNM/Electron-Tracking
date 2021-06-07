#ifndef SERunAction_h
#define SERunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class SEEventAction;
class G4Run;

/// Run action class
///

class SERunAction : public G4UserRunAction
{
public:
  SERunAction(SEEventAction* eventAction, G4String name);
  virtual ~SERunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  SEEventAction*   fEventAction;  // have event information for run
  G4String         fout;          // output file name
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
