#include "BIWatchHit.hh"

G4ThreadLocal G4Allocator<BIWatchHit>* BIWatchHitAllocator=0;

BIWatchHit::BIWatchHit()
 : G4VHit(),
   fHid(0),
   fTime(0.),
   fPos(G4ThreeVector()),
   fMom(G4ThreeVector()),
   fKine(0.)
{}

BIWatchHit::~BIWatchHit() {}

BIWatchHit::BIWatchHit(const BIWatchHit& right)
  : G4VHit()
{
  fHid       = right.fHid;
  fTime      = right.fTime;
  fPos       = right.fPos;
  fMom       = right.fMom;
  fKine      = right.fKine;
}

const BIWatchHit& BIWatchHit::operator=(const BIWatchHit& right)
{
  fHid       = right.fHid;
  fTime      = right.fTime;
  fPos       = right.fPos;
  fMom       = right.fMom;
  fKine      = right.fKine;

  return *this;
}

G4bool BIWatchHit::operator==(const BIWatchHit& right) const
{
  return ( this == &right ) ? true : false;
}

void BIWatchHit::Draw()
{ ; }

void BIWatchHit::Print()
{ ; }

