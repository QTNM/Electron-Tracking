#ifndef SEEventAction_h
#define SEEventAction_h 1

#include <vector>

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

  // to create columns for Ntuple
  std::vector<G4int>&    GetHitID()     { return hitid; }
  std::vector<G4int>&    GetExitID()    { return exitid; }
  std::vector<G4int>&    GetParentID()  { return pid; }
  std::vector<G4double>& GetHitKinE()   { return kine; }
  std::vector<G4double>& GetHitTime()   { return thit; }
  std::vector<G4double>& GetExitTime()  { return texit; }

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
  std::vector<G4int>    hitid;
  std::vector<G4int>    exitid;
  std::vector<G4int>    pid;
  std::vector<G4double> kine;
  std::vector<G4double> thit;
  std::vector<G4double> texit;

};

#endif
