// SEPSBoundaryTrackID
#include "SEPSBoundaryTrackID.hh"
#include "G4UnitsTable.hh"

////////////////////////////////////////////////////////////////////////////////
// Description:
//   This is a primitive scorer class for scoring TrackID at Boundary
//
///////////////////////////////////////////////////////////////////////////////

SEPSBoundaryTrackID::SEPSBoundaryTrackID(G4String name, G4int depth)
: G4VPrimitiveScorer(std::move(name), depth)
, HCID(-1)
, EvtMap(nullptr)
{
  SetUnit("NoUnit");
}

SEPSBoundaryTrackID::~SEPSBoundaryTrackID() = default;

G4bool SEPSBoundaryTrackID::ProcessHits(G4Step* aStep, G4TouchableHistory* /*unused*/)
{
  G4StepPoint* preStep = aStep->GetPreStepPoint();
  G4bool IsEnter = preStep->GetStepStatus()==fGeomBoundary;

  if (!IsEnter) return false; // boundary check

  G4int          index = GetIndex(aStep);
  G4TrackLogger& tlog  = fCellTrackLogger[index];
  if(tlog.FirstEnterance(aStep->GetTrack()->GetTrackID()))
  {
    G4int tid = aStep->GetTrack()->GetTrackID();
    EvtMap->add(index, tid);
  }
  return true;
}

void SEPSBoundaryTrackID::Initialize(G4HCofThisEvent* HCE)
{
  EvtMap = new G4THitsMap<G4int>(GetMultiFunctionalDetector()->GetName(), GetName());
  if(HCID < 0)
  {
    HCID = GetCollectionID(0);
  }
  HCE->AddHitsCollection(HCID, (G4VHitsCollection*) EvtMap);
}

void SEPSBoundaryTrackID::EndOfEvent(G4HCofThisEvent* /*unused*/) { fCellTrackLogger.clear(); }

void SEPSBoundaryTrackID::clear()
{
  fCellTrackLogger.clear();
  EvtMap->clear();
}

void SEPSBoundaryTrackID::DrawAll() { ; }

void SEPSBoundaryTrackID::PrintAll()
{
  G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
  G4cout << " PrimitiveScorer " << GetName() << G4endl;
  G4cout << " Number of entries " << EvtMap->entries() << G4endl;
  for(auto itr : *(EvtMap->GetMap()))
  {
    G4cout << "  key: " << itr.first << "  track ID: " << *(itr.second) << G4endl;
  }
}

void SEPSBoundaryTrackID::SetUnit(const G4String& unit) { ; }
