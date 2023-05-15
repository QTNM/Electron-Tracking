#ifndef BIEventAction_h
#define BIEventAction_h 1

#include "BIGasHit.hh"
#include "BIWatchHit.hh"

#include "G4RootAnalysisManager.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"

/// Event action class
///

class BIEventAction : public G4UserEventAction
{
  using G4AnalysisManager = G4RootAnalysisManager;

public:
  BIEventAction()          = default;
  virtual ~BIEventAction() = default;

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

private:
  // methods
  BIGasHitsCollection*     GetGasHitsCollection(G4int hcID,
                                              const G4Event* event) const;

  BIWatchHitsCollection*     GetWatchHitsCollection(G4int hcID,
                                              const G4Event* event) const;

  // data members
  // hit data
  G4int                 fGID    = -1;
  G4int                 fWID    = -1;

};

#endif
