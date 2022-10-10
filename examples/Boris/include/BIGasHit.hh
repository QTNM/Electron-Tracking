#ifndef BIGasHit_h
#define BIGasHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

/// Gas hit class
///
/// It defines data members to store the energy deposit,
/// kinetic energy and momentum in a selected volume:

class BIGasHit : public G4VHit
{
  public:
    BIGasHit();
    BIGasHit(const BIGasHit&);
    virtual ~BIGasHit();

    // operators
    const BIGasHit& operator=(const BIGasHit&);
    G4bool operator==(const BIGasHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw();
    virtual void Print();

    // Set methods
    void SetTrackID     (G4int id)     { fTrackID = id; };
    void SetEdep        (G4double de)  { fEdep = de; };
    void SetKine        (G4double ke)  { fKine = ke; };
    void SetPx          (G4double px)  { fPx = px; };
    void SetPy          (G4double py)  { fPy = py; };
    void SetPz          (G4double pz)  { fPz = pz; };
    void SetPosx        (G4double lx)  { fPosx = lx; };
    void SetPosy        (G4double ly)  { fPosy = ly; };
    void SetPosz        (G4double lz)  { fPosz = lz; };

    // Get methods
    G4double GetTrackID() const     { return fTrackID; };
    G4double GetEdep()    const     { return fEdep; };
    G4double GetKine()    const     { return fKine; };
    G4double GetPx()      const     { return fPx; };
    G4double GetPy()      const     { return fPy; };
    G4double GetPz()      const     { return fPz; };
    G4double GetPosx()    const     { return fPosx; };
    G4double GetPosy()    const     { return fPosy; };
    G4double GetPosz()    const     { return fPosz; };

  private:

      G4int         fTrackID;
      G4double      fEdep;
      G4double      fKine;
      G4double      fPx;
      G4double      fPy;
      G4double      fPz;
      G4double      fPosx;
      G4double      fPosy;
      G4double      fPosz;
};

typedef G4THitsCollection<BIGasHit> BIGasHitsCollection;

extern G4ThreadLocal G4Allocator<BIGasHit>* BIGasHitAllocator;

inline void* BIGasHit::operator new(size_t)
{
  if(!BIGasHitAllocator)
      BIGasHitAllocator = new G4Allocator<BIGasHit>;
  return (void *) BIGasHitAllocator->MallocSingle();
}

inline void BIGasHit::operator delete(void *hit)
{
  BIGasHitAllocator->FreeSingle((BIGasHit*) hit);
}

#endif
