#include "CDEventAction.hh"

#include <vector>

#include "G4Event.hh"
#include "G4AnalysisManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "CDGasSD.hh"


CDGasHitsCollection* 
CDEventAction::GetGasHitsCollection(G4int hcID,
                                    const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<CDGasHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("CDEventAction::GetGasHitsCollection()",
      "MyCode0001", FatalException, msg);
  }         

  return hitsCollection;
}    


void CDEventAction::BeginOfEventAction(const G4Event*
                                         /*event*/)
{ ; }

void CDEventAction::EndOfEventAction(const G4Event* event)
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
  std::vector<double> tkine, px, py, pz, posx, posy, posz;
  std::vector<int> tid, tpdg;

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill Hits output from SD
  G4int GnofHits = GasHC->entries();

  // Gas detector
  for ( G4int i=0; i<GnofHits; i++ ) 
  {
    auto hh = (*GasHC)[i];

    int    id = (hh->GetTrackID());
    int    p  = (hh->GetPDG());
    double k  = (hh->GetKine()) / G4Analysis::GetUnitValue("keV");
    double mx = (hh->GetPx()); // normalised momentum direction vector
    double my = (hh->GetPy());
    double mz = (hh->GetPz());
    double lx = (hh->GetPosx()) / G4Analysis::GetUnitValue("mm");
    double ly = (hh->GetPosy()) / G4Analysis::GetUnitValue("mm");
    double lz = (hh->GetPosz()) / G4Analysis::GetUnitValue("mm");

    tid.push_back(id);
    tpdg.push_back(p);
    tkine.push_back(k);
    px.push_back(mx);
    py.push_back(my);
    pz.push_back(mz);
    posx.push_back(lx);
    posy.push_back(ly);
    posz.push_back(lz);
  }

  // fill the ntuple - check column id?
  G4int eventID = event->GetEventID();
  for (unsigned int i=0;i<tkine.size();i++)
  {
    analysisManager->FillNtupleIColumn(0, eventID); // repeat all rows
    analysisManager->FillNtupleIColumn(1, tid.at(i));
    analysisManager->FillNtupleIColumn(2, tpdg.at(i));
    analysisManager->FillNtupleDColumn(3, tkine.at(i));
    analysisManager->FillNtupleDColumn(4, px.at(i));
    analysisManager->FillNtupleDColumn(5, py.at(i));
    analysisManager->FillNtupleDColumn(6, pz.at(i));
    analysisManager->FillNtupleDColumn(7, posx.at(i));
    analysisManager->FillNtupleDColumn(8, posy.at(i));
    analysisManager->FillNtupleDColumn(9, posz.at(i));
    analysisManager->AddNtupleRow();
  }

  // printing
  // G4cout << ">>> Event: " << eventID << G4endl;
  // G4cout << "    " << GnofHits << " gas hits stored in this event." << G4endl;
}
