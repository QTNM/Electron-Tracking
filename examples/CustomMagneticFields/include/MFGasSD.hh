#ifndef MFGasSD_h
#define MFGasSD_h 1

#include "G4VSensitiveDetector.hh"

#include "MFGasHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

/// MFGas sensitive detector class
///
/// The hits are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step. A hit is created with each step with non zero 
/// energy deposit.

class MFGasSD : public G4VSensitiveDetector
{
  public:
    MFGasSD(const G4String& name, 
            const G4String& hitsCollectionName);
    virtual ~MFGasSD();
  
    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

  private:
    MFGasHitsCollection* fHitsCollection;
};

#endif

