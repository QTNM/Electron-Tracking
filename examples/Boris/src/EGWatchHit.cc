#include "EGWatchHit.hh"

G4ThreadLocal G4Allocator<EGWatchHit>* EGWatchHitAllocator=0;

EGWatchHit::EGWatchHit()
 : G4VHit(),
   fHid(0),
   fTime(0.),
   fPos(G4ThreeVector()),
   fMom(G4ThreeVector()),
   fKine(0.)
{}

EGWatchHit::~EGWatchHit() {}

EGWatchHit::EGWatchHit(const EGWatchHit& right)
  : G4VHit()
{
  fHid       = right.fHid;
  fTime      = right.fTime;
  fPos       = right.fPos;
  fMom       = right.fMom;
  fKine      = right.fKine;
}

const EGWatchHit& EGWatchHit::operator=(const EGWatchHit& right)
{
  fHid       = right.fHid;
  fTime      = right.fTime;
  fPos       = right.fPos;
  fMom       = right.fMom;
  fKine      = right.fKine;

  return *this;
}

G4bool EGWatchHit::operator==(const EGWatchHit& right) const
{
  return ( this == &right ) ? true : false;
}

void EGWatchHit::Draw()
{ ; }

void EGWatchHit::Print()
{ ; }

