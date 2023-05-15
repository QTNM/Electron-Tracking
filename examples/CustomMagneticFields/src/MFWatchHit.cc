#include "MFWatchHit.hh"

G4ThreadLocal G4Allocator<MFWatchHit>* MFWatchHitAllocator=0;

MFWatchHit::MFWatchHit()
 : G4VHit(),
   fHid(0),
   fTime(0.),
   fPos(G4ThreeVector()),
   fMom(G4ThreeVector()),
   fKine(0.)
{}

MFWatchHit::~MFWatchHit() {}

MFWatchHit::MFWatchHit(const MFWatchHit& right)
  : G4VHit()
{
  fHid       = right.fHid;
  fTime      = right.fTime;
  fPos       = right.fPos;
  fMom       = right.fMom;
  fKine      = right.fKine;
}

const MFWatchHit& MFWatchHit::operator=(const MFWatchHit& right)
{
  fHid       = right.fHid;
  fTime      = right.fTime;
  fPos       = right.fPos;
  fMom       = right.fMom;
  fKine      = right.fKine;

  return *this;
}

G4bool MFWatchHit::operator==(const MFWatchHit& right) const
{
  return ( this == &right ) ? true : false;
}

void MFWatchHit::Draw()
{ ; }

void MFWatchHit::Print()
{ ; }

