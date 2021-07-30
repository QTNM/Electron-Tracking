#ifndef EGGasHit_h
#define EGGasHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

/// Gas hit class
///
/// It defines data members to store the energy deposit,
/// and momentum in a selected volume:

class EGGasHit : public G4VHit
{
  public:
    EGGasHit();
    EGGasHit(const EGGasHit&);
    virtual ~EGGasHit();

    // operators
    const EGGasHit& operator=(const EGGasHit&);
    G4bool operator==(const EGGasHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw();
    virtual void Print();

    // Set methods
    void SetEdep     (G4double de)  { fEdep = de; };

    // Get methods
    G4double GetEdep() const     { return fEdep; };

  private:

      G4double      fEdep;
};

typedef G4THitsCollection<EGGasHit> EGGasHitsCollection;

extern G4ThreadLocal G4Allocator<EGGasHit>* EGGasHitAllocator;

inline void* EGGasHit::operator new(size_t)
{
  if(!EGGasHitAllocator)
      EGGasHitAllocator = new G4Allocator<EGGasHit>;
  return (void *) EGGasHitAllocator->MallocSingle();
}

inline void EGGasHit::operator delete(void *hit)
{
  EGGasHitAllocator->FreeSingle((EGGasHit*) hit);
}

#endif

