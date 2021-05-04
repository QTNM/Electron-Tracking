#include "SEGasHit.hh"

G4ThreadLocal G4Allocator<SEGasHit>* SEGasHitAllocator=0;

SEGasHit::SEGasHit()
 : G4VHit(),
   fTid(0),
   fTime(0.),
   fKine(0.),
   fEdep(0.),
   fPid(0)
{}

SEGasHit::~SEGasHit() {}

SEGasHit::SEGasHit(const SEGasHit& right)
  : G4VHit()
{
  fPid       = right.fPid;
  fTid       = right.fTid;
  fTime      = right.fTime;
  fEdep      = right.fEdep;
  fKine      = right.fKine;
}

const SEGasHit& SEGasHit::operator=(const SEGasHit& right)
{
  fPid       = right.fPid;
  fTid       = right.fTid;
  fTime      = right.fTime;
  fEdep      = right.fEdep;
  fKine      = right.fKine;

  return *this;
}

G4bool SEGasHit::operator==(const SEGasHit& right) const
{
  return ( this == &right ) ? true : false;
}

void SEGasHit::Draw()
{ ; }

void SEGasHit::Print()
{ ; }

