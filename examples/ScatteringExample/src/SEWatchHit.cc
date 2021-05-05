#include "SEWatchHit.hh"

G4ThreadLocal G4Allocator<SEWatchHit>* SEWatchHitAllocator=0;

SEWatchHit::SEWatchHit()
 : G4VHit(),
   fHid(0),
   fTime(0.),
   fPos(G4ThreeVector())
{}

SEWatchHit::~SEWatchHit() {}

SEWatchHit::SEWatchHit(const SEWatchHit& right)
  : G4VHit()
{
  fHid       = right.fHid;
  fTime      = right.fTime;
  fPos       = right.fPos;
}

const SEWatchHit& SEWatchHit::operator=(const SEWatchHit& right)
{
  fHid       = right.fHid;
  fTime      = right.fTime;
  fPos       = right.fPos;

  return *this;
}

G4bool SEWatchHit::operator==(const SEWatchHit& right) const
{
  return ( this == &right ) ? true : false;
}

void SEWatchHit::Draw()
{ ; }

void SEWatchHit::Print()
{ ; }

