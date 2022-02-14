#include "EGGasSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

EGGasSD::EGGasSD(const G4String& name,
                 const G4String& hitsCollectionName) 
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL)
{
  collectionName.insert(hitsCollectionName);
}

EGGasSD::~EGGasSD() 
{}

void EGGasSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection 
    = new EGGasHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}

G4bool EGGasSD::ProcessHits(G4Step* aStep, 
                            G4TouchableHistory*)
{  
  // energy deposit
  G4double edep = aStep->GetTotalEnergyDeposit();
  G4ThreeVector premom  = aStep->GetPreStepPoint()->GetMomentumDirection();
  G4ThreeVector postmom = aStep->GetPostStepPoint()->GetMomentumDirection();

  if (edep / CLHEP::keV <= 1.e-6) return false;
  else if ((premom.cross(postmom)).mag() <= 1.e-8) return false; // parallel = not interested

  EGGasHit* newHit = new EGGasHit();
  G4ThreeVector postloc = aStep->GetPostStepPoint()->GetPosition();

  newHit->SetTrackID(aStep->GetTrack()->GetTrackID());
  newHit->SetEdep(edep);
  newHit->SetKine(aStep->GetPostStepPoint()->GetKineticEnergy());
  newHit->SetPx(postmom.x());
  newHit->SetPy(postmom.y());
  newHit->SetPz(postmom.z());
  newHit->SetPosx(postloc.x());
  newHit->SetPosy(postloc.y());
  newHit->SetPosz(postloc.z());

  fHitsCollection->insert( newHit );

  return true;
}

void EGGasSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
     G4int nofHits = fHitsCollection->entries();
     G4cout << G4endl
            << "-------->Hits Collection: in this event there are " << nofHits 
            << " hits in the gas: " << G4endl;
  }
}


