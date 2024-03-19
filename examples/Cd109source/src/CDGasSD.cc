#include "CDGasSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4StepStatus.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

CDGasSD::CDGasSD(const G4String& name,
                 const G4String& hitsCollectionName) 
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL)
{
  collectionName.insert(hitsCollectionName);
}

CDGasSD::~CDGasSD() 
{}

void CDGasSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection 
    = new CDGasHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}

G4bool CDGasSD::ProcessHits(G4Step* aStep, 
                            G4TouchableHistory*)
{  
  auto* prestep  = aStep->GetPreStepPoint();
  auto* poststep = aStep->GetPostStepPoint();

  // test; check for boundary
  if (poststep->GetStepStatus() != fGeomBoundary) return false;
    
  // test; check correct boundary, vacuum to vacuum only at scoring surface
  if (prestep->GetMaterial()->GetName() != poststep->GetMaterial()->GetName()) return false;
  
  CDGasHit* newHit = new CDGasHit();
  G4ThreeVector postmom = aStep->GetPostStepPoint()->GetMomentumDirection();
  G4ThreeVector postloc = aStep->GetPostStepPoint()->GetPosition();

  newHit->SetTrackID(aStep->GetTrack()->GetTrackID());
  newHit->SetPDG(aStep->GetTrack()->GetDynamicParticle()->GetPDGcode());
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

void CDGasSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
     G4int nofHits = fHitsCollection->entries();
     G4cout << G4endl
            << "-------->Hits Collection: in this event there are " << nofHits 
            << " scoring surface crossings. " << G4endl;
  }
}


