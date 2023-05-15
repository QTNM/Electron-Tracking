#include "MFGasHit.hh"

G4ThreadLocal G4Allocator<MFGasHit>* MFGasHitAllocator=0;

MFGasHit::MFGasHit()
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

MFGasHit::~MFGasHit() {}

MFGasHit::MFGasHit(const MFGasHit& right)
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

const MFGasHit& MFGasHit::operator=(const MFGasHit& right)
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

G4bool MFGasHit::operator==(const MFGasHit& right) const
{
  return ( this == &right ) ? true : false;
}

void MFGasHit::Draw()
{ ; }

void MFGasHit::Print()
{ ; }

