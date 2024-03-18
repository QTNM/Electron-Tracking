#ifndef CDGasSD_h
#define CDGasSD_h 1

#include "G4VSensitiveDetector.hh"

#include "CDGasHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

/// CDGas sensitive detector class
///
/// The hits are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step. A hit is created with each step crossing the
/// scoring boundary.

class CDGasSD : public G4VSensitiveDetector
{
  public:
    CDGasSD(const G4String& name, 
            const G4String& hitsCollectionName);
    virtual ~CDGasSD();
  
    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

  private:
    CDGasHitsCollection* fHitsCollection;
};

#endif

