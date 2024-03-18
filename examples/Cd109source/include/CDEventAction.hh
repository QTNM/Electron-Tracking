#ifndef CDEventAction_h
#define CDEventAction_h 1

#include "CDGasHit.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

/// Event action class
///

class CDEventAction : public G4UserEventAction
{
public:
  CDEventAction()          = default;
  virtual ~CDEventAction() = default;

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

private:
  // methods
  CDGasHitsCollection*     GetGasHitsCollection(G4int hcID,
                                              const G4Event* event) const;

  // data members
  // hit data
  G4int                 fGID    = -1;

};

#endif
