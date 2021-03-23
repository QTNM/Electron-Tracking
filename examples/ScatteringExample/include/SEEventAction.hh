#ifndef SEEventAction_h
#define SEEventAction_h 1

#include "G4THitsMap.hh"
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
  G4THitsMap<G4int>*         GetIntHitsCollection(G4int hcID, const G4Event* event) const;
  G4THitsMap<G4double>*      GetHitsCollection(G4int hcID, const G4Event* event) const;


  // data members
  // hit data
  G4int                 fTidID    = -1;
  G4int                 fPidID    = -1;
  G4int                 fTimeID   = -1;
  G4int                 fKinEID   = -1;
  G4int                 fExitTID  = -1;
  G4int                 fExitidID = -1;

};

#endif
