// WLGDPSTrackID
#include "WLGDPSTrackID.hh"
#include "G4UnitsTable.hh"

////////////////////////////////////////////////////////////////////////////////
// Description:
//   This is a primitive scorer class for scoring TrackID
//
///////////////////////////////////////////////////////////////////////////////

WLGDPSTrackID::WLGDPSTrackID(G4String name, G4int depth)
: G4VPrimitiveScorer(std::move(name), depth)
, HCID(-1)
, EvtMap(nullptr)
{
  SetUnit("NoUnit");
}

WLGDPSTrackID::~WLGDPSTrackID() = default;

G4bool WLGDPSTrackID::ProcessHits(G4Step* aStep, G4TouchableHistory* /*unused*/)
{
  if(aStep->GetTotalEnergyDeposit() <= 0.0)
    return false;

  G4int          index = GetIndex(aStep);
  G4TrackLogger& tlog  = fCellTrackLogger[index];
  if(tlog.FirstEnterance(aStep->GetTrack()->GetTrackID()))
  {
    G4int tid = aStep->GetTrack()->GetTrackID();
    EvtMap->add(index, tid);
  }
  return true;
}

void WLGDPSTrackID::Initialize(G4HCofThisEvent* HCE)
{
  EvtMap = new G4THitsMap<G4int>(GetMultiFunctionalDetector()->GetName(), GetName());
  if(HCID < 0)
  {
    HCID = GetCollectionID(0);
  }
  HCE->AddHitsCollection(HCID, (G4VHitsCollection*) EvtMap);
}

void WLGDPSTrackID::EndOfEvent(G4HCofThisEvent* /*unused*/) { fCellTrackLogger.clear(); }

void WLGDPSTrackID::clear()
{
  fCellTrackLogger.clear();
  EvtMap->clear();
}

void WLGDPSTrackID::DrawAll() { ; }

void WLGDPSTrackID::PrintAll()
{
  G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
  G4cout << " PrimitiveScorer " << GetName() << G4endl;
  G4cout << " Number of entries " << EvtMap->entries() << G4endl;
  for(auto itr : *(EvtMap->GetMap()))
  {
    G4cout << "  key: " << itr.first << "  track ID: " << *(itr.second) << G4endl;
  }
}

void WLGDPSTrackID::SetUnit(const G4String& unit) { ; }
