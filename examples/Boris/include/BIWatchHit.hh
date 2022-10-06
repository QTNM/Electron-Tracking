#ifndef BIWatchHit_h
#define BIWatchHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

/// Stop Watch hit class
///
/// It defines data members to store the track ID, Time
/// and position on crossing the stop watch boundary:

class BIWatchHit : public G4VHit
{
  public:
    BIWatchHit();
    BIWatchHit(const BIWatchHit&);
    virtual ~BIWatchHit();

    // operators
    const BIWatchHit& operator=(const BIWatchHit&);
    G4bool operator==(const BIWatchHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw();
    virtual void Print();

    // Set methods
    void SetHid      (G4int    id)      { fHid  = id; };
    void SetTime     (G4double ti)      { fTime = ti; };
    void SetKine        (G4double ke)  { fKine = ke; };
    void SetPos      (G4ThreeVector xyz){ fPos = xyz; };
    void SetMom      (G4ThreeVector xyz){ fMom = xyz; };

    // Get methods
    G4int    GetHID() const      { return fHid; };
    G4double GetTime() const     { return fTime; };
    G4double GetKine()    const     { return fKine; };
    G4ThreeVector GetPos() const { return fPos; };
    G4ThreeVector GetMom() const { return fMom; };

  private:

      G4int         fHid;
      G4double      fTime;
      G4double      fKine;
      G4ThreeVector fPos;
      G4ThreeVector fMom;
};

typedef G4THitsCollection<BIWatchHit> BIWatchHitsCollection;

extern G4ThreadLocal G4Allocator<BIWatchHit>* BIWatchHitAllocator;

inline void* BIWatchHit::operator new(size_t)
{
  if(!BIWatchHitAllocator)
      BIWatchHitAllocator = new G4Allocator<BIWatchHit>;
  return (void *) BIWatchHitAllocator->MallocSingle();
}

inline void BIWatchHit::operator delete(void *hit)
{
  BIWatchHitAllocator->FreeSingle((BIWatchHit*) hit);
}

#endif

