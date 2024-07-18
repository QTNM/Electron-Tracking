#ifndef PEEventAction_h
#define PEEventAction_h 1

#include "PEGasHit.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

/// Event action class
///

class PEEventAction : public G4UserEventAction
{
public:
  PEEventAction()          = default;
  virtual ~PEEventAction() = default;

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

private:
  // methods
  PEGasHitsCollection*     GetGasHitsCollection(G4int hcID,
						const G4Event* event) const;

  // data members
  G4int                 fGID    = -1;

};

#endif
