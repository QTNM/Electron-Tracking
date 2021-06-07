#ifndef SEWatchHit_h
#define SEWatchHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

/// Stop Watch hit class
///
/// It defines data members to store the track ID, Time
/// and position on crossing the stop watch boundary:

class SEWatchHit : public G4VHit
{
  public:
    SEWatchHit();
    SEWatchHit(const SEWatchHit&);
    virtual ~SEWatchHit();

    // operators
    const SEWatchHit& operator=(const SEWatchHit&);
    G4bool operator==(const SEWatchHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw();
    virtual void Print();

    // Set methods
    void SetHid      (G4int    id)      { fHid  = id; };
    void SetTime     (G4double ti)      { fTime = ti; };
    void SetPos      (G4ThreeVector xyz){ fPos = xyz; };

    // Get methods
    G4int    GetHID() const      { return fHid; };
    G4double GetTime() const     { return fTime; };
    G4ThreeVector GetPos() const { return fPos; };

  private:

      G4int         fHid;
      G4double      fTime;
      G4ThreeVector fPos;
};

typedef G4THitsCollection<SEWatchHit> SEWatchHitsCollection;

extern G4ThreadLocal G4Allocator<SEWatchHit>* SEWatchHitAllocator;

inline void* SEWatchHit::operator new(size_t)
{
  if(!SEWatchHitAllocator)
      SEWatchHitAllocator = new G4Allocator<SEWatchHit>;
  return (void *) SEWatchHitAllocator->MallocSingle();
}

inline void SEWatchHit::operator delete(void *hit)
{
  SEWatchHitAllocator->FreeSingle((SEWatchHit*) hit);
}

#endif

