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
    void SetTrackID     (G4int id)     { fTrackID = id; };
    void SetEdep        (G4double de)  { fEdep = de; };
    void SetPx          (G4double px)  { fPx = px; };
    void SetPy          (G4double py)  { fPy = py; };
    void SetPz          (G4double pz)  { fPz = pz; };

    // Get methods
    G4double GetTrackID() const     { return fTrackID; };
    G4double GetEdep()    const     { return fEdep; };
    G4double GetPx()      const     { return fPx; };
    G4double GetPy()      const     { return fPy; };
    G4double GetPz()      const     { return fPz; };

  private:

      G4int         fTrackID;
      G4double      fEdep;
      G4double      fPx;
      G4double      fPy;
      G4double      fPz;
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

