#include "EGGasHit.hh"

G4ThreadLocal G4Allocator<EGGasHit>* EGGasHitAllocator=0;

EGGasHit::EGGasHit()
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

EGGasHit::~EGGasHit() {}

EGGasHit::EGGasHit(const EGGasHit& right)
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

const EGGasHit& EGGasHit::operator=(const EGGasHit& right)
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

G4bool EGGasHit::operator==(const EGGasHit& right) const
{
  return ( this == &right ) ? true : false;
}

void EGGasHit::Draw()
{ ; }

void EGGasHit::Print()
{ ; }

