#ifndef EGEventAction_h
#define EGEventAction_h 1

#include "EGGasHit.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

/// Event action class
///

class EGEventAction : public G4UserEventAction
{
public:
  EGEventAction()          = default;
  virtual ~EGEventAction() = default;

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

private:
  // methods
  EGGasHitsCollection*     GetGasHitsCollection(G4int hcID,
                                              const G4Event* event) const;

  // data members
  // hit data
  G4int                 fGID    = -1;

};

#endif
