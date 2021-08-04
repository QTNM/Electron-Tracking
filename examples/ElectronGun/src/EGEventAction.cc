#include "EGEventAction.hh"
#include "g4root.hh"

#include <vector>

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "EGGasSD.hh"


EGGasHitsCollection* 
EGEventAction::GetGasHitsCollection(G4int hcID,
                                    const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<EGGasHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("EGEventAction::GetGasHitsCollection()",
      "MyCode0001", FatalException, msg);
  }         

  return hitsCollection;
}    


void EGEventAction::BeginOfEventAction(const G4Event*
                                         /*event*/)
{ ; }

void EGEventAction::EndOfEventAction(const G4Event* event)
{
  // Get GAS hits collections IDs
  if(fGID < 0) 
    fGID = G4SDManager::GetSDMpointer()->GetCollectionID("GasHitsCollection");

  // Get entries from hits collections
  //
  auto GasHC     = GetGasHitsCollection(fGID, event);

  if(GasHC->entries() <= 0)
  {
    return;  // no action on no hit
  }

  // dummy storage
  std::vector<double> tedep, tkine, px, py, pz;
  std::vector<int> tid;

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill Hits output from SD
  G4int GnofHits = GasHC->entries();

  // Gas detector
  for ( G4int i=0; i<GnofHits; i++ ) 
  {
    auto hh = (*GasHC)[i];

    int    id = (hh->GetTrackID());
    double e  = (hh->GetEdep())    / G4Analysis::GetUnitValue("keV");
    double k  = (hh->GetKine())    / G4Analysis::GetUnitValue("keV");
    double mx = (hh->GetPx()); // normalised momentum direction vector
    double my = (hh->GetPy());
    double mz = (hh->GetPz());

    tid.push_back(id);
    tedep.push_back(e);
    tkine.push_back(k);
    px.push_back(mx);
    py.push_back(my);
    pz.push_back(mz);
  }

  // fill the ntuple - check column id?
  G4int eventID = event->GetEventID();
  for (unsigned int i=0;i<tedep.size();i++)
  {
    analysisManager->FillNtupleIColumn(0, eventID); // repeat all rows
    analysisManager->FillNtupleIColumn(1, tid.at(i));
    analysisManager->FillNtupleDColumn(2, tedep.at(i));
    analysisManager->FillNtupleDColumn(3, tkine.at(i));
    analysisManager->FillNtupleDColumn(4, px.at(i));
    analysisManager->FillNtupleDColumn(5, py.at(i));
    analysisManager->FillNtupleDColumn(6, pz.at(i));
    analysisManager->AddNtupleRow();
  }

  // printing
//  G4cout << ">>> Event: " << eventID << G4endl;
//  G4cout << "    " << GnofHits << " gas hits stored in this event." << G4endl;
}
