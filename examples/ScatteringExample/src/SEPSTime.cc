// SEPSTime
#include "SEPSTime.hh"
#include "G4UnitsTable.hh"

////////////////////////////////////////////////////////////////////////////////
// Description:
//   This is a primitive scorer class for scoring global track time
//
///////////////////////////////////////////////////////////////////////////////

SEPSTime::SEPSTime(G4String name, G4int depth)
: G4VPrimitiveScorer(std::move(name), depth)
, HCID(-1)
, EvtMap(nullptr)
{
  SetUnit("ns");
}

SEPSTime::SEPSTime(G4String name, const G4String& unit, G4int depth)
: G4VPrimitiveScorer(std::move(name), depth)
, HCID(-1)
, EvtMap(nullptr)
{
  SetUnit(unit);
}

SEPSTime::~SEPSTime() = default;

G4bool SEPSTime::ProcessHits(G4Step* aStep, G4TouchableHistory* /*unused*/)
{
  if(aStep->GetTotalEnergyDeposit() / CLHEP::keV <= 1.e-7) // 100 micro eV min.
    return false;

  G4int          index = aStep->GetTrack()->GetCurrentStepNumber(); // unique key
  // global time since start of event
  G4double tt = aStep->GetTrack()->GetGlobalTime();

  EvtMap->add(index, tt);
  return true;
}

void SEPSTime::Initialize(G4HCofThisEvent* HCE)
{
  EvtMap = new G4THitsMap<G4double>(GetMultiFunctionalDetector()->GetName(), GetName());
  if(HCID < 0)
  {
    HCID = GetCollectionID(0);
  }
  HCE->AddHitsCollection(HCID, (G4VHitsCollection*) EvtMap);
}

void SEPSTime::EndOfEvent(G4HCofThisEvent* /*unused*/) { ; }

void SEPSTime::clear()
{
  EvtMap->clear();
}

void SEPSTime::DrawAll() { ; }

void SEPSTime::PrintAll()
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

void SEPSTime::SetUnit(const G4String& unit) { CheckAndSetUnit(unit, "Time"); }
