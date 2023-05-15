#ifndef MFEventAction_h
#define MFEventAction_h 1

#include "MFGasHit.hh"
#include "MFWatchHit.hh"
#include "G4RootAnalysisManager.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"

/// Event action class
///

class MFEventAction : public G4UserEventAction
{
  using G4AnalysisManager = G4RootAnalysisManager;

public:
  MFEventAction()          = default;
  virtual ~MFEventAction() = default;

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

private:
  // methods
  MFGasHitsCollection*     GetGasHitsCollection(G4int hcID,
                                              const G4Event* event) const;

  MFWatchHitsCollection*     GetWatchHitsCollection(G4int hcID,
                                              const G4Event* event) const;

  // data members
  // hit data
  G4int                 fGID    = -1;
  G4int                 fWID    = -1;

};

#endif
