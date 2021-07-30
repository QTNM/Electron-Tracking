#include "SEGasSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

SEGasSD::SEGasSD(const G4String& name,
                 const G4String& hitsCollectionName) 
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL)
{
  collectionName.insert(hitsCollectionName);
}

SEGasSD::~SEGasSD() 
{}

void SEGasSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection 
    = new SEGasHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}

G4bool SEGasSD::ProcessHits(G4Step* aStep, 
                            G4TouchableHistory*)
{  
  // energy deposit
  G4double edep = aStep->GetTotalEnergyDeposit();

  if (edep / CLHEP::keV <= 1.e-6) return false;

  SEGasHit* newHit = new SEGasHit();

  newHit->SetParentID(aStep->GetTrack()->GetParentID());
  newHit->SetTrackID(aStep->GetTrack()->GetTrackID());
  newHit->SetTime(aStep->GetTrack()->GetGlobalTime());
  newHit->SetEdep(edep);
  newHit->SetKine(aStep->GetPostStepPoint()->GetKineticEnergy());

  fHitsCollection->insert( newHit );

  return true;
}

void SEGasSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
     G4int nofHits = fHitsCollection->entries();
     G4cout << G4endl
            << "-------->Hits Collection: in this event there are " << nofHits 
            << " hits in the gas: " << G4endl;
  }
}


