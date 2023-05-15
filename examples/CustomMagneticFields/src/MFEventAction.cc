#include "MFEventAction.hh"

#include <vector>

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "MFGasSD.hh"
#include "MFWatchSD.hh"


MFGasHitsCollection* 
MFEventAction::GetGasHitsCollection(G4int hcID,
                                    const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<MFGasHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("MFEventAction::GetGasHitsCollection()",
      "MyCode0001", FatalException, msg);
  }         

  return hitsCollection;
}    

MFWatchHitsCollection*
MFEventAction::GetWatchHitsCollection(G4int hcID,
                                    const G4Event* event) const
{
  auto hitsCollection
    = static_cast<MFWatchHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));

  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID;
    G4Exception("MFEventAction::GetWatchHitsCollection()",
      "MyCode0001", FatalException, msg);
  }

  return hitsCollection;
}

void MFEventAction::BeginOfEventAction(const G4Event*
                                         /*event*/)
{ ; }

void MFEventAction::EndOfEventAction(const G4Event* event)
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

  if(GasHC->entries() <= 0  && WatchHC->entries() <= 0)
  {
    return;  // no action on no hit
  }

  // dummy storage
  std::vector<double> tedep, tkine, px, py, pz, posx, posy, posz;
  std::vector<double> w_tex, w_posx, w_posy, w_posz;
  std::vector<double> w_px, w_py, w_pz, w_tkine;
  std::vector<int> tid, w_tid;

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill Hits output from SD
  G4int GnofHits = GasHC->entries();
  G4int WnofHits = WatchHC->entries();

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
    double lx = (hh->GetPosx()); // interaction location
    double ly = (hh->GetPosy());
    double lz = (hh->GetPosz());

    tid.push_back(id);
    tedep.push_back(e);
    tkine.push_back(k);
    px.push_back(mx);
    py.push_back(my);
    pz.push_back(mz);
    posx.push_back(lx);
    posy.push_back(ly);
    posz.push_back(lz);
  }

  // Watch detector
  for ( G4int i=0; i<WnofHits; i++ )
  {
    auto hh = (*WatchHC)[i];

    int   id =  hh->GetHID();
    double t = (hh->GetTime())    / G4Analysis::GetUnitValue("ns");
    double x = (hh->GetPos()).x() / G4Analysis::GetUnitValue("mm");
    double y = (hh->GetPos()).y() / G4Analysis::GetUnitValue("mm");
    double z = (hh->GetPos()).z() / G4Analysis::GetUnitValue("mm");
    double mx = (hh->GetMom().x()); // normalised momentum direction vector
    double my = (hh->GetMom()).y();
    double mz = (hh->GetMom().z());
    double k  = (hh->GetKine())    / G4Analysis::GetUnitValue("keV");

    w_tid.push_back(id);
    w_tex.push_back(t);
    w_posx.push_back(x);
    w_posy.push_back(y);
    w_posz.push_back(z);
    w_px.push_back(mx);
    w_py.push_back(my);
    w_pz.push_back(mz);
    w_tkine.push_back(k);
  }

  // fill the ntuple - check column id?
  G4int eventID = event->GetEventID();
  for (unsigned int i=0;i<tedep.size();i++)
  {
    analysisManager->FillNtupleIColumn(0, 0, eventID); // repeat all rows
    analysisManager->FillNtupleIColumn(0, 1, tid.at(i));
    analysisManager->FillNtupleDColumn(0, 2, tedep.at(i));
    analysisManager->FillNtupleDColumn(0, 3, tkine.at(i));
    analysisManager->FillNtupleDColumn(0, 4, px.at(i));
    analysisManager->FillNtupleDColumn(0, 5, py.at(i));
    analysisManager->FillNtupleDColumn(0, 6, pz.at(i));
    analysisManager->FillNtupleDColumn(0, 7, posx.at(i));
    analysisManager->FillNtupleDColumn(0, 8, posy.at(i));
    analysisManager->FillNtupleDColumn(0, 9, posz.at(i));
    analysisManager->AddNtupleRow();
  }

  for (unsigned int i=0;i<w_tid.size();i++)
  {
    analysisManager->FillNtupleIColumn(1, 0, eventID); // repeat all rows
    analysisManager->FillNtupleIColumn(1, 1, w_tid.at(i));
    analysisManager->FillNtupleDColumn(1, 2, w_tex.at(i));
    analysisManager->FillNtupleDColumn(1, 3, w_posx.at(i));
    analysisManager->FillNtupleDColumn(1, 4, w_posy.at(i));
    analysisManager->FillNtupleDColumn(1, 5, w_posz.at(i));
    analysisManager->FillNtupleDColumn(1, 6, w_px.at(i));
    analysisManager->FillNtupleDColumn(1, 7, w_py.at(i));
    analysisManager->FillNtupleDColumn(1, 8, w_pz.at(i));
    analysisManager->FillNtupleDColumn(1, 9, w_tkine.at(i));
    analysisManager->AddNtupleRow(1);
  }

  // printing
  G4cout << ">>> Event: " << eventID << G4endl;
  G4cout << "    " << GnofHits << " gas hits stored in this event." << G4endl;
  G4cout << "    " << WnofHits << " stopwatch hits stored in this event." << G4endl;
}
