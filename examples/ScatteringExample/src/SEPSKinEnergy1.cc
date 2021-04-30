// SEPSKinEnergy1
#include "SEPSKinEnergy1.hh"
#include "G4UnitsTable.hh"

////////////////////////////////////////////////////////////////////////////////
// Description:
//   This is a primitive scorer class for scoring pre-step kinetic energy 
//
///////////////////////////////////////////////////////////////////////////////

SEPSKinEnergy1::SEPSKinEnergy1(G4String name, G4int depth)
: G4VPrimitiveScorer(std::move(name), depth)
, HCID(-1)
, EvtMap(nullptr)
{
  SetUnit("keV");
}

SEPSKinEnergy1::SEPSKinEnergy1(G4String name, const G4String& unit, G4int depth)
: G4VPrimitiveScorer(std::move(name), depth)
, HCID(-1)
, EvtMap(nullptr)
{
  SetUnit(unit);
}

SEPSKinEnergy1::~SEPSKinEnergy1() = default;

G4bool SEPSKinEnergy1::ProcessHits(G4Step* aStep, G4TouchableHistory* /*unused*/)
{
  G4double edep = aStep->GetTotalEnergyDeposit();

  if(edep / GetUnitValue() > 1.e-6) // 1 meV minimum
  {
    G4double kinetic = aStep->GetPreStepPoint()->GetKineticEnergy();
    G4int index = aStep->GetTrack()->GetCurrentStepNumber(); // unique key
    // debug
    // G4cout << "in kine: " << index << ", " << kinetic / GetUnitValue() << G4endl;
    EvtMap->add(index, kinetic);
  }
  else
    return false;

  return true;
}

void SEPSKinEnergy1::Initialize(G4HCofThisEvent* HCE)
{
  EvtMap = new G4THitsMap<G4double>(GetMultiFunctionalDetector()->GetName(), GetName());
  if(HCID < 0)
  {
    HCID = GetCollectionID(0);
  }
  HCE->AddHitsCollection(HCID, (G4VHitsCollection*) EvtMap);
}

void SEPSKinEnergy1::EndOfEvent(G4HCofThisEvent* /*unused*/) { ; }

void SEPSKinEnergy1::clear() { EvtMap->clear(); }

void SEPSKinEnergy1::DrawAll() { ; }

void SEPSKinEnergy1::PrintAll()
{
  G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
  G4cout << " PrimitiveScorer " << GetName() << G4endl;
  G4cout << " Number of entries " << EvtMap->entries() << G4endl;
  auto itr = EvtMap->GetMap()->begin();
  for(; itr != EvtMap->GetMap()->end(); itr++)
  {
    G4cout << "  key: " << itr->first
           << "  kinetic energy: " << *(itr->second) / GetUnitValue() << " [" << GetUnit()
           << "]" << G4endl;
  }
}

void SEPSKinEnergy1::SetUnit(const G4String& unit)
{
  CheckAndSetUnit(unit, "Energy");
}
