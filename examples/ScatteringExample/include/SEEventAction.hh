#ifndef SEEventAction_h
#define SEEventAction_h 1

#include "SEGasHit.hh"
#include "SEWatchHit.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

/// Event action class
///

class SEEventAction : public G4UserEventAction
{
public:
  SEEventAction()          = default;
  virtual ~SEEventAction() = default;

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

private:
  // methods
  SEGasHitsCollection*     GetGasHitsCollection(G4int hcID,
                                              const G4Event* event) const;

  SEWatchHitsCollection*   GetWatchHitsCollection(G4int hcID,
                                                  const G4Event* event) const;

  // data members
  // hit data
  G4int                 fGID    = -1;
  G4int                 fWID    = -1;

};

#endif
