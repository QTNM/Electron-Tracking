#include "EGWatchSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

EGWatchSD::EGWatchSD(const G4String& name,
                     const G4String& hitsCollectionName) 
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL)
{
  collectionName.insert(hitsCollectionName);
}

EGWatchSD::~EGWatchSD() 
{}

void EGWatchSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection 
    = new EGWatchHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool EGWatchSD::ProcessHits(G4Step* aStep, 
                              G4TouchableHistory*)
{  
  // boundary crossing
  G4StepPoint* preStep = aStep->GetPreStepPoint();
  G4bool IsEnter = preStep->GetStepStatus() == fGeomBoundary;

  if (!IsEnter) return false; // boundary check

  EGWatchHit* newHit = new EGWatchHit();

  newHit->SetHid(aStep->GetTrack()->GetTrackID());
  newHit->SetTime(aStep->GetTrack()->GetGlobalTime());
  newHit->SetPos(aStep->GetPreStepPoint()->GetPosition());
  newHit->SetMom(aStep->GetPreStepPoint()->GetMomentumDirection());
  newHit->SetKine(aStep->GetPostStepPoint()->GetKineticEnergy());

  fHitsCollection->insert( newHit );

  return true;
}

void EGWatchSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
     G4int nofHits = fHitsCollection->entries();
     G4cout << G4endl
            << "-------->Hits Collection: in this event there are " << nofHits 
            << " hits in the stop watch: " << G4endl;
  }
}


