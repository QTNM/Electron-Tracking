// SEPSParentID
#include "SEPSParentID.hh"
#include "G4UnitsTable.hh"

////////////////////////////////////////////////////////////////////////////////
// Description:
//   This is a primitive scorer class for scoring ParentID
//
///////////////////////////////////////////////////////////////////////////////

SEPSParentID::SEPSParentID(G4String name, G4int depth)
: G4VPrimitiveScorer(std::move(name), depth)
, HCID(-1)
, EvtMap(nullptr)
{
  SetUnit("NoUnit");
}

SEPSParentID::~SEPSParentID() = default;

G4bool SEPSParentID::ProcessHits(G4Step* aStep, G4TouchableHistory* /*unused*/)
{
  if(aStep->GetTotalEnergyDeposit()  / CLHEP::keV <= 1.e-6)
    return false;

  G4int          index = GetIndex(aStep);
  G4TrackLogger& tlog  = fCellTrackLogger[index];
  if(tlog.FirstEnterance(aStep->GetTrack()->GetParentID()))
  {
    G4int pid = aStep->GetTrack()->GetParentID();
    EvtMap->add(index, pid);
  }
  return true;
}

void SEPSParentID::Initialize(G4HCofThisEvent* HCE)
{
  EvtMap = new G4THitsMap<G4int>(GetMultiFunctionalDetector()->GetName(), GetName());
  if(HCID < 0)
  {
    HCID = GetCollectionID(0);
  }
  HCE->AddHitsCollection(HCID, (G4VHitsCollection*) EvtMap);
}

void SEPSParentID::EndOfEvent(G4HCofThisEvent* /*unused*/) { fCellTrackLogger.clear(); }

void SEPSParentID::clear()
{
  fCellTrackLogger.clear();
  EvtMap->clear();
}

void SEPSParentID::DrawAll() { ; }

void SEPSParentID::PrintAll()
{
  G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
  G4cout << " PrimitiveScorer " << GetName() << G4endl;
  G4cout << " Number of entries " << EvtMap->entries() << G4endl;
  for(auto itr : *(EvtMap->GetMap()))
  {
    G4cout << "  key: " << itr.first << "  parent ID: " << *(itr.second) << G4endl;
  }
}

void SEPSParentID::SetUnit(const G4String& unit) { ; }
