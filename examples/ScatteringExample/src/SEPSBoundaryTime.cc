// SEPSBoundaryTime
#include "SEPSBoundaryTime.hh"
#include "G4UnitsTable.hh"

////////////////////////////////////////////////////////////////////////////////
// Description:
//   This is a primitive scorer class for scoring global track time at Boundary
//
///////////////////////////////////////////////////////////////////////////////

SEPSBoundaryTime::SEPSBoundaryTime(G4String name, G4int depth)
: G4VPrimitiveScorer(std::move(name), depth)
, HCID(-1)
, EvtMap(nullptr)
{
  SetUnit("ns");
}

SEPSBoundaryTime::SEPSBoundaryTime(G4String name, const G4String& unit, G4int depth)
: G4VPrimitiveScorer(std::move(name), depth)
, HCID(-1)
, EvtMap(nullptr)
{
  SetUnit(unit);
}

SEPSBoundaryTime::~SEPSBoundaryTime() = default;

G4bool SEPSBoundaryTime::ProcessHits(G4Step* aStep, G4TouchableHistory* /*unused*/)
{
  G4StepPoint* preStep = aStep->GetPreStepPoint();
  G4bool IsEnter = preStep->GetStepStatus()==fGeomBoundary;

  if (!IsEnter) return false;

  G4int          index = GetIndex(aStep);
  // global time since start of event
  G4double tt = aStep->GetTrack()->GetGlobalTime();

  // debug
  // G4cout << "in BdTime: index, time" << index << ", " << tt / GetUnitValue() << G4endl;

  EvtMap->add(index, tt);
  return true;
}

void SEPSBoundaryTime::Initialize(G4HCofThisEvent* HCE)
{
  EvtMap = new G4THitsMap<G4double>(GetMultiFunctionalDetector()->GetName(), GetName());
  if(HCID < 0)
  {
    HCID = GetCollectionID(0);
  }
  HCE->AddHitsCollection(HCID, (G4VHitsCollection*) EvtMap);
}

void SEPSBoundaryTime::EndOfEvent(G4HCofThisEvent* /*unused*/) { ; }

void SEPSBoundaryTime::clear()
{
  EvtMap->clear();
}

void SEPSBoundaryTime::DrawAll() { ; }

void SEPSBoundaryTime::PrintAll()
{
  G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
  G4cout << " PrimitiveScorer " << GetName() << G4endl;
  G4cout << " Number of entries " << EvtMap->entries() << G4endl;
  auto itr = EvtMap->GetMap()->begin();
  for(; itr != EvtMap->GetMap()->end(); itr++)
  {
    G4cout << "  key: " << itr->first
           << "  global time: " << *(itr->second) / GetUnitValue() << " [" << GetUnit()
           << "]" << G4endl;
  }
}

void SEPSBoundaryTime::SetUnit(const G4String& unit) { CheckAndSetUnit(unit, "Time"); }
