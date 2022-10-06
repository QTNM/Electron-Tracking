#include "BIGasHit.hh"

G4ThreadLocal G4Allocator<BIGasHit>* BIGasHitAllocator=0;

BIGasHit::BIGasHit()
 : G4VHit(),
   fTrackID(0),
   fEdep(0.),
   fKine(0.),
   fPx(0.),
   fPy(0.),
   fPz(0.),
   fPosx(0.),
   fPosy(0.),
   fPosz(0.)
{}

BIGasHit::~BIGasHit() {}

BIGasHit::BIGasHit(const BIGasHit& right)
  : G4VHit()
{
  fTrackID      = right.fTrackID;
  fEdep         = right.fEdep;
  fKine         = right.fKine;
  fPx           = right.fPx;
  fPy           = right.fPy;
  fPz           = right.fPz;
  fPosx         = right.fPosx;
  fPosy         = right.fPosy;
  fPosz         = right.fPosz;
}

const BIGasHit& BIGasHit::operator=(const BIGasHit& right)
{
  fTrackID      = right.fTrackID;
  fEdep         = right.fEdep;
  fKine         = right.fKine;
  fPx           = right.fPx;
  fPy           = right.fPy;
  fPz           = right.fPz;
  fPosx         = right.fPosx;
  fPosy         = right.fPosy;
  fPosz         = right.fPosz;

  return *this;
}

G4bool BIGasHit::operator==(const BIGasHit& right) const
{
  return ( this == &right ) ? true : false;
}

void BIGasHit::Draw()
{ ; }

void BIGasHit::Print()
{ ; }

