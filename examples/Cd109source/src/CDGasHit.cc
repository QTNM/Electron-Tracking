#include "CDGasHit.hh"

G4ThreadLocal G4Allocator<CDGasHit>* CDGasHitAllocator=0;

CDGasHit::CDGasHit()
 : G4VHit(),
   fTrackID(0),
   fPDG(0),
   fKine(0.),
   fPx(0.),
   fPy(0.),
   fPz(0.),
   fPosx(0.),
   fPosy(0.),
   fPosz(0.)
{}

CDGasHit::~CDGasHit() {}

CDGasHit::CDGasHit(const CDGasHit& right)
  : G4VHit()
{
  fTrackID      = right.fTrackID;
  fPDG          = right.fPDG;
  fKine         = right.fKine;
  fPx           = right.fPx;
  fPy           = right.fPy;
  fPz           = right.fPz;
  fPosx         = right.fPosx;
  fPosy         = right.fPosy;
  fPosz         = right.fPosz;
}

const CDGasHit& CDGasHit::operator=(const CDGasHit& right)
{
  fTrackID      = right.fTrackID;
  fPDG          = right.fPDG;
  fKine         = right.fKine;
  fPx           = right.fPx;
  fPy           = right.fPy;
  fPz           = right.fPz;
  fPosx         = right.fPosx;
  fPosy         = right.fPosy;
  fPosz         = right.fPosz;

  return *this;
}

G4bool CDGasHit::operator==(const CDGasHit& right) const
{
  return ( this == &right ) ? true : false;
}

void CDGasHit::Draw()
{ ; }

void CDGasHit::Print()
{ ; }

