#include "SEEventAction.hh"
#include "g4root.hh"

#include <vector>

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
{ ; }

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

  // dummy storage
  std::vector<double> tkin, ttime, tex;
  std::vector<int> hid, pid, texid;

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill Hits output from SD
  for(auto it : *KinEMap->GetMap())
  {
    double kin = (*it.second) / G4Analysis::GetUnitValue("keV");
    tkin.push_back(kin);
  }
  for(auto it : *TimeMap->GetMap())
  {
    double th = (*it.second) / G4Analysis::GetUnitValue("ns");
    ttime.push_back(th);
  }
  for(auto it : *TidMap->GetMap())
  {
    hid.push_back((*it.second));
  }
  for(auto it : *PidMap->GetMap())
  {
    pid.push_back((*it.second));
  }

  // fill stopwatch output
  for(auto it : *ExidMap->GetMap())
  {
    texid.push_back((*it.second));
  }
  for(auto it : *ExTMap->GetMap())
  {
    double ext = (*it.second) / G4Analysis::GetUnitValue("ns");
    tex.push_back(ext);
  }

  // fill the ntuple - check column id?
  G4int eventID = event->GetEventID();
  for (unsigned int i=0;i<tkin.size();i++)
  {
    analysisManager->FillNtupleIColumn(0, 0, eventID); // repeat all rows
    analysisManager->FillNtupleDColumn(0, 1, tkin.at(i));
    analysisManager->FillNtupleDColumn(0, 2, ttime.at(i)); // same size
    if (i >= hid.size()) 
    {
      analysisManager->FillNtupleIColumn(0, 3, hid.back()); // repeat
      analysisManager->FillNtupleIColumn(0, 4, pid.back());
    }
    else {
      analysisManager->FillNtupleIColumn(0, 3, hid.at(i));
      analysisManager->FillNtupleIColumn(0, 4, pid.at(i));
    }
    analysisManager->AddNtupleRow(0);
  }
  for (unsigned int i=0;i<texid.size();i++)
  {
    analysisManager->FillNtupleIColumn(1, 0, eventID); // repeat all rows
    analysisManager->FillNtupleIColumn(1, 1, texid.at(i));
    analysisManager->FillNtupleDColumn(1, 2, tex.at(i));
    analysisManager->AddNtupleRow(1);
  }

  // printing
  G4cout << ">>> Event: " << eventID << G4endl;
  if (TidMap->entries())
    G4cout << "    " << TidMap->entries() << " gas hits stored in this event." << G4endl;
  if (ExidMap->entries())
    G4cout << "    " << ExidMap->entries() << " stopwatch hits stored in this event." << G4endl;
}
