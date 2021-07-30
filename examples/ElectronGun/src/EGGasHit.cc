#include "EGGasHit.hh"

G4ThreadLocal G4Allocator<EGGasHit>* EGGasHitAllocator=0;

EGGasHit::EGGasHit()
 : G4VHit(),
   fEdep(0.),
{}

EGGasHit::~EGGasHit() {}

EGGasHit::EGGasHit(const EGGasHit& right)
  : G4VHit()
{
  fEdep      = right.fEdep;
}

const EGGasHit& EGGasHit::operator=(const EGGasHit& right)
{
  fEdep      = right.fEdep;

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

