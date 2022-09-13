#ifndef EGWatchSD_h
#define EGWatchSD_h 1

#include "G4VSensitiveDetector.hh"

#include "EGWatchHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

/// EGWatch sensitive detector class
///
/// The hits are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step. A hit is created with each step with non zero 
/// energy deposit.

class EGWatchSD : public G4VSensitiveDetector
{
  public:
    EGWatchSD(const G4String& name, 
              const G4String& hitsCollectionName);
    virtual ~EGWatchSD();
  
    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

  private:
    EGWatchHitsCollection* fHitsCollection;
};

#endif

