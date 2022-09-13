#ifndef EGWatchHit_h
#define EGWatchHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

/// Stop Watch hit class
///
/// It defines data members to store the track ID, Time
/// and position on crossing the stop watch boundary:

class EGWatchHit : public G4VHit
{
  public:
    EGWatchHit();
    EGWatchHit(const EGWatchHit&);
    virtual ~EGWatchHit();

    // operators
    const EGWatchHit& operator=(const EGWatchHit&);
    G4bool operator==(const EGWatchHit&) const;

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

typedef G4THitsCollection<EGWatchHit> EGWatchHitsCollection;

extern G4ThreadLocal G4Allocator<EGWatchHit>* EGWatchHitAllocator;

inline void* EGWatchHit::operator new(size_t)
{
  if(!EGWatchHitAllocator)
      EGWatchHitAllocator = new G4Allocator<EGWatchHit>;
  return (void *) EGWatchHitAllocator->MallocSingle();
}

inline void EGWatchHit::operator delete(void *hit)
{
  EGWatchHitAllocator->FreeSingle((EGWatchHit*) hit);
}

#endif

