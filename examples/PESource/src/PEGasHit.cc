#include "PEGasHit.hh"

G4ThreadLocal G4Allocator<PEGasHit>* PEGasHitAllocator=0;

PEGasHit::PEGasHit()
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

PEGasHit::~PEGasHit() {}

PEGasHit::PEGasHit(const PEGasHit& right)
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

const PEGasHit& PEGasHit::operator=(const PEGasHit& right)
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

G4bool PEGasHit::operator==(const PEGasHit& right) const
{
  return ( this == &right ) ? true : false;
}

void PEGasHit::Draw()
{ ; }

void PEGasHit::Print()
{ ; }

