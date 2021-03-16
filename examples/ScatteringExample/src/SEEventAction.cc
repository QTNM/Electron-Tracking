#include "SEEventAction.hh"
#include "SETrajectory.hh"
#include "g4root.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"

#include "Randomize.hh"
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <vector>

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
  hitpdg.clear();
  kine.clear();
  thit.clear();
}

void SEEventAction::EndOfEventAction(const G4Event* event)
{
  // Get hist collections IDs
  if(fTidID < 0)
  {
    fTidID    = G4SDManager::GetSDMpointer()->GetCollectionID("Det/TrackID");
    fPidID    = G4SDManager::GetSDMpointer()->GetCollectionID("Det/ParentID");
    fHitPDG   = G4SDManager::GetSDMpointer()->GetCollectionID("Det/HitPDG");
    fTimeID   = G4SDManager::GetSDMpointer()->GetCollectionID("Det/Time");
    fKinEID   = G4SDManager::GetSDMpointer()->GetCollectionID("Det/KinE");
  }

  // Get entries from hits collections
  //
  G4THitsMap<G4int>*         TidMap    = GetIntHitsCollection(fTidID, event);
  G4THitsMap<G4int>*         PidMap    = GetIntHitsCollection(fPidID, event);
  G4THitsMap<G4int>*         PDGMap    = GetIntHitsCollection(fHitPDG, event);
  G4THitsMap<G4double>*      KinEMap   = GetHitsCollection(fKinEID, event);
  G4THitsMap<G4double>*      TimeMap   = GetHitsCollection(fTimeID, event);

  if(TidMap->entries() <= 0)
  {
    return;  // no action on no hit
  }

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill Hits output from SD
  for(auto it : *HitsMap->GetMap())
  {
    edep.push_back((*it.second) / G4Analysis::GetUnitValue("MeV"));
  }
  for(auto it : *TimeMap->GetMap())
  {
    thit.push_back((*it.second) / G4Analysis::GetUnitValue("ns"));
  }
  for(auto it : *WeightMap->GetMap())
  {
    whit.push_back((*it.second));
  }
  for(auto it : *LocMap->GetMap())
  {
    xloc.push_back((*it.second).x() / G4Analysis::GetUnitValue("m"));
    yloc.push_back((*it.second).y() / G4Analysis::GetUnitValue("m"));
    zloc.push_back((*it.second).z() / G4Analysis::GetUnitValue("m"));
  }
  for(auto it : *THitsMap->GetMap())
  {
    htrid.push_back((*it.second));
  }

  // fill the ntuple
  analysisManager->AddNtupleRow();

  // printing
  G4int eventID = event->GetEventID();
  G4cout << ">>> Event: " << eventID << G4endl;
  G4cout << "    " << edep.size() << " hits stored in this event." << G4endl;
  G4cout << "    " << trjpdg.size() << " trajectories stored in this event." << G4endl;
}
