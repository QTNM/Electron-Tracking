#include "SEWatchSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

SEWatchSD::SEWatchSD(const G4String& name,
                     const G4String& hitsCollectionName) 
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL)
{
  collectionName.insert(hitsCollectionName);
}

SEWatchSD::~SEWatchSD() 
{}

void SEWatchSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection 
    = new SEWatchHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool SEWatchSD::ProcessHits(G4Step* aStep, 
                              G4TouchableHistory*)
{  
  // energy deposit
  G4double edep = aStep->GetTotalEnergyDeposit();

  if (edep / CLHEP::keV <= 1.e-6) return false;

  SEWatchHit* newHit = new SEWatchHit();

  newHit->SetHid(aStep->GetTrack()->GetTrackID());
  newHit->SetTime(aStep->GetTrack()->GetGlobalTime());
  newHit->SetPos(aStep->GetPreStepPoint()->GetPosition());

  fHitsCollection->insert( newHit );

  return true;
}

void SEWatchSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
     G4int nofHits = fHitsCollection->entries();
     G4cout << G4endl
            << "-------->Hits Collection: in this event there are " << nofHits 
            << " hits in the stop watch: " << G4endl;
  }
}


