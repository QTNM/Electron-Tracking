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
    fEdepID   = G4SDManager::GetSDMpointer()->GetCollectionID("Gas/Edep");
    fKin1EID  = G4SDManager::GetSDMpointer()->GetCollectionID("Gas/KinE1");
    fKin2EID  = G4SDManager::GetSDMpointer()->GetCollectionID("Gas/KinE2");
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
  G4THitsMap<G4double>*      EdepMap   = GetHitsCollection(fEdepID, event);
  G4THitsMap<G4double>*      Kin1EMap  = GetHitsCollection(fKin1EID, event);
  G4THitsMap<G4double>*      Kin2EMap  = GetHitsCollection(fKin2EID, event);
  G4THitsMap<G4double>*      TimeMap   = GetHitsCollection(fTimeID, event);

  G4THitsMap<G4int>*         ExidMap   = GetIntHitsCollection(fExitidID, event);
  G4THitsMap<G4double>*      ExTMap    = GetHitsCollection(fExitTID, event);

  if(TidMap->entries() <= 0 && ExidMap->entries() <= 0)
  {
    return;  // no action on no hit
  }

  // dummy storage
  std::vector<double> tkin1, tkin2, tedep, ttime, tex;
  std::vector<int> hid, pid, texid;

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill Hits output from SD
  for(auto it : *Kin1EMap->GetMap())
  {
    double kin = (*it.second) / G4Analysis::GetUnitValue("keV");
    tkin1.push_back(kin);
  }
  for(auto it : *Kin2EMap->GetMap())
  { 
    double kin = (*it.second) / G4Analysis::GetUnitValue("keV");
    tkin2.push_back(kin);
  }
  for(auto it : *EdepMap->GetMap())
  { 
    double ed = (*it.second) / G4Analysis::GetUnitValue("keV");
    tedep.push_back(ed);
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
  for (unsigned int i=0;i<tedep.size();i++)
  {
    analysisManager->FillNtupleIColumn(0, 0, eventID); // repeat all rows
    analysisManager->FillNtupleDColumn(0, 1, tedep.at(i));
    analysisManager->FillNtupleDColumn(0, 2, tkin1.at(i));
    analysisManager->FillNtupleDColumn(0, 3, tkin2.at(i));
    analysisManager->FillNtupleDColumn(0, 4, ttime.at(i)); // same size
    if (i >= hid.size()) 
    {
      analysisManager->FillNtupleIColumn(0, 5, hid.back()); // repeat
      analysisManager->FillNtupleIColumn(0, 6, pid.back());
    }
    else {
      analysisManager->FillNtupleIColumn(0, 5, hid.at(i));
      analysisManager->FillNtupleIColumn(0, 6, pid.at(i));
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
  //G4cout << ">>> Event: " << eventID << G4endl;
  //if (TidMap->entries())
  //  G4cout << "    " << TidMap->entries() << " gas hits stored in this event." << G4endl;
  //if (ExidMap->entries())
  //  G4cout << "    " << ExidMap->entries() << " stopwatch hits stored in this event." << G4endl;
}
