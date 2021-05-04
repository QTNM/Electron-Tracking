#include "SEEventAction.hh"
#include "g4root.hh"

#include <vector>

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "SEGasSD.hh"
#include "SEWatchSD.hh"


SEGasHitsCollection* 
SEEventAction::GetGasHitsCollection(G4int hcID,
                                    const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<SEGasHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("SEEventAction::GetGasHitsCollection()",
      "MyCode0001", FatalException, msg);
  }         

  return hitsCollection;
}    


SEWatchHitsCollection* 
SEEventAction::GetWatchHitsCollection(G4int hcID,
                                    const G4Event* event) const
{   
  auto hitsCollection
    = static_cast<SEWatchHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));

  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID;
    G4Exception("SEEventAction::GetWatchHitsCollection()",
      "MyCode0001", FatalException, msg);
  }

  return hitsCollection;
}
                                    

void SEEventAction::BeginOfEventAction(const G4Event*
                                         /*event*/)
{ ; }

void SEEventAction::EndOfEventAction(const G4Event* event)
{
  // Get GAS hits collections IDs
  if(fGID < 0)
    fGID = G4SDManager::GetSDMpointer()->GetCollectionID("GasHitsCollection");

  if(fWID<0)
    fWID = G4SDManager::GetSDMpointer()->GetCollectionID("WatchHitsCollection");
  

  // Get entries from hits collections
  //
  auto GasHC     = GetGasHitsCollection(fGID, event);
  auto WatchHC   = GetWatchHitsCollection(fWID, event);

  if(GasHC->entries() <= 0 && WatchHC->entries() <= 0)
  {
    return;  // no action on no hit
  }

  // dummy storage
  std::vector<double> tkin, tedep, ttime, tex, xp, yp;
  std::vector<int> hid, pid, texid;

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill Hits output from SD
  G4int GnofHits = GasHC->entries();
  G4int WnofHits = WatchHC->entries();

  // Gas detector
  for ( G4int i=0; i<GnofHits; i++ ) 
  {
    auto hh = (*GasHC)[i];

    double e = (hh->GetEdep())    / G4Analysis::GetUnitValue("keV");
    double t = (hh->GetTime())    / G4Analysis::GetUnitValue("ns");
    double k = (hh->GetKine())    / G4Analysis::GetUnitValue("keV");
    int id1  =  hh->GetTrackID();
    int id2  =  hh->GetParentID();

    tedep.push_back(e);
    tkin.push_back(k);
    ttime.push_back(t);
    hid.push_back(id1);
    pid.push_back(id2);
  }
  // Watch detector
  for ( G4int i=0; i<WnofHits; i++ )
  {
    auto hh = (*WatchHC)[i];
    
    int   id =  hh->GetHID();
    double t = (hh->GetTime())    / G4Analysis::GetUnitValue("ns");
    double x = (hh->GetPos()).x() / G4Analysis::GetUnitValue("mm");
    double y = (hh->GetPos()).y() / G4Analysis::GetUnitValue("mm");
  
    texid.push_back(id);
    tex.push_back(t);
    xp.push_back(x);
    yp.push_back(y);
  }

  // fill the ntuple - check column id?
  G4int eventID = event->GetEventID();
  for (unsigned int i=0;i<tedep.size();i++)
  {
    analysisManager->FillNtupleIColumn(0, 0, eventID); // repeat all rows
    analysisManager->FillNtupleDColumn(0, 1, tedep.at(i));
    analysisManager->FillNtupleDColumn(0, 2, tkin.at(i));
    analysisManager->FillNtupleDColumn(0, 3, ttime.at(i)); // same size
    if (i >= hid.size()) 
    {
      analysisManager->FillNtupleIColumn(0, 4, hid.back()); // repeat
      analysisManager->FillNtupleIColumn(0, 5, pid.back());
    }
    else {
      analysisManager->FillNtupleIColumn(0, 4, hid.at(i));
      analysisManager->FillNtupleIColumn(0, 5, pid.at(i));
    }
    analysisManager->AddNtupleRow(0);
  }
  for (unsigned int i=0;i<texid.size();i++)
  {
    analysisManager->FillNtupleIColumn(1, 0, eventID); // repeat all rows
    analysisManager->FillNtupleIColumn(1, 1, texid.at(i));
    analysisManager->FillNtupleDColumn(1, 2, tex.at(i));
    analysisManager->FillNtupleDColumn(1, 3, xp.at(i));
    analysisManager->FillNtupleDColumn(1, 4, yp.at(i));
    analysisManager->AddNtupleRow(1);
  }

  // printing
  //G4cout << ">>> Event: " << eventID << G4endl;
  //  G4cout << "    " << GnofHits << " gas hits stored in this event." << G4endl;
  //  G4cout << "    " << WnofHits << " stopwatch hits stored in this event." << G4endl;
}
