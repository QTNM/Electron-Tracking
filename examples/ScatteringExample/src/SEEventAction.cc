#include "SEEventAction.hh"
#include "g4root.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"


G4THitsMap<G4int>* SEEventAction::GetIntHitsCollection(G4int          hcID,
                                                         const G4Event* event) const
{
  auto hitsCollection =
    static_cast<G4THitsMap<G4int>*>(event->GetHCofThisEvent()->GetHC(hcID));

  if(hitsCollection == nullptr)
  {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID;
    G4Exception("SEEventAction::GetHitsCollection()", "MyCode0003", FatalException,
                msg);
  }

  return hitsCollection;
}

G4THitsMap<G4double>* SEEventAction::GetHitsCollection(G4int          hcID,
                                                         const G4Event* event) const
{
  auto hitsCollection =
    static_cast<G4THitsMap<G4double>*>(event->GetHCofThisEvent()->GetHC(hcID));

  if(hitsCollection == nullptr)
  {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID;
    G4Exception("SEEventAction::GetHitsCollection()", "MyCode0003", FatalException,
                msg);
  }

  return hitsCollection;
}

void SEEventAction::BeginOfEventAction(const G4Event*
                                         /*event*/)
{
  hitid.clear();
  pid.clear();
  kine.clear();
  thit.clear();
  texit.clear();
  exitid.clear();
}

void SEEventAction::EndOfEventAction(const G4Event* event)
{
  // Get GAS hits collections IDs
  if(fTidID < 0)
  {
    fTidID    = G4SDManager::GetSDMpointer()->GetCollectionID("Gas/TrackID");
    fPidID    = G4SDManager::GetSDMpointer()->GetCollectionID("Gas/PID");
    fTimeID   = G4SDManager::GetSDMpointer()->GetCollectionID("Gas/Time");
    fKinEID   = G4SDManager::GetSDMpointer()->GetCollectionID("Gas/KinE");
  }
  if(fExitidID<0)
  {
    fExitidID = G4SDManager::GetSDMpointer()->GetCollectionID("Stop/ExitID");
    fExitTID  = G4SDManager::GetSDMpointer()->GetCollectionID("Stop/ExitT");
  }

  // Get entries from hits collections
  //
  G4THitsMap<G4int>*         TidMap    = GetIntHitsCollection(fTidID, event);
  G4THitsMap<G4int>*         PidMap    = GetIntHitsCollection(fPidID, event);
  G4THitsMap<G4double>*      KinEMap   = GetHitsCollection(fKinEID, event);
  G4THitsMap<G4double>*      TimeMap   = GetHitsCollection(fTimeID, event);

  G4THitsMap<G4int>*         ExidMap   = GetIntHitsCollection(fExitidID, event);
  G4THitsMap<G4double>*      ExTMap    = GetHitsCollection(fExitTID, event);

  if(TidMap->entries() <= 0 && ExidMap->entries() <= 0)
  {
    return;  // no action on no hit
  }

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill Hits output from SD
  for(auto it : *KinEMap->GetMap())
  {
    kine.push_back((*it.second) / G4Analysis::GetUnitValue("keV"));
  }
  for(auto it : *TimeMap->GetMap())
  {
    thit.push_back((*it.second) / G4Analysis::GetUnitValue("ns"));
  }
  for(auto it : *TidMap->GetMap())
  {
    hitid.push_back((*it.second));
  }
  for(auto it : *PidMap->GetMap())
  {
    pid.push_back((*it.second).x();
  }

  // fill stopwatch output
  for(auto it : *ExTMap->GetMap())
  {
    exitid.push_back((*it.second));
  }
  for(auto it : *ExidMap->GetMap())
  {
    texit.push_back((*it.second));
  }

  // fill the ntuple
  analysisManager->AddNtupleRow();

  // printing
  G4int eventID = event->GetEventID();
  G4cout << ">>> Event: " << eventID << G4endl;
  G4cout << "    " << hitid.size() << " gas hits stored in this event." << G4endl;
  G4cout << "    " << exitid.size() << " stopwatch hits stored in this event." << G4endl;
}
