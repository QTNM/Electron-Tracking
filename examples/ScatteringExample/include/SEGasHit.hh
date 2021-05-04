#ifndef SEGasHit_h
#define SEGasHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

/// Gas hit class
///
/// It defines data members to store the energy deposit,
/// and position in a selected volume:

class SEGasHit : public G4VHit
{
  public:
    SEGasHit();
    SEGasHit(const SEGasHit&);
    virtual ~SEGasHit();

    // operators
    const SEGasHit& operator=(const SEGasHit&);
    G4bool operator==(const SEGasHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw();
    virtual void Print();

    // Set methods
    void SetParentID (G4int id)     { fPid  = id; };
    void SetTrackID  (G4int id)     { fTid  = id; };
    void SetEdep     (G4double de)  { fEdep = de; };
    void SetTime     (G4double ti)  { fTime = ti; };
    void SetKine     (G4double ke)  { fKine = ke; };

    // Get methods
    G4int    GetParentID() const { return fPid; };
    G4int    GetTrackID()  const { return fTid; };
    G4double GetEdep() const     { return fEdep; };
    G4double GetTime() const     { return fTime; };
    G4double GetKine() const     { return fKine; };

  private:

      G4int         fTid;
      G4int         fPid;
      G4double      fEdep;
      G4double      fTime;
      G4double      fKine;
};

typedef G4THitsCollection<SEGasHit> SEGasHitsCollection;

extern G4ThreadLocal G4Allocator<SEGasHit>* SEGasHitAllocator;

inline void* SEGasHit::operator new(size_t)
{
  if(!SEGasHitAllocator)
      SEGasHitAllocator = new G4Allocator<SEGasHit>;
  return (void *) SEGasHitAllocator->MallocSingle();
}

inline void SEGasHit::operator delete(void *hit)
{
  SEGasHitAllocator->FreeSingle((SEGasHit*) hit);
}

#endif

