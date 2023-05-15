#include "MFMagneticTrap.hh"

MFMagneticTrap::MFMagneticTrap(const G4ThreeVector& FieldVector )
{
  fFieldComponents[0] = FieldVector.x();
  fFieldComponents[1] = FieldVector.y();
  fFieldComponents[2] = FieldVector.z();
}

void
MFMagneticTrap::SetFieldValue(const G4ThreeVector& newFieldVector )
{
  fFieldComponents[0] = newFieldVector.x();
  fFieldComponents[1] = newFieldVector.y();
  fFieldComponents[2] = newFieldVector.z();
}

MFMagneticTrap::~MFMagneticTrap()
{
}

MFMagneticTrap::MFMagneticTrap (const MFMagneticTrap &p)
  : G4MagneticField(p)
{
  for (G4int i=0; i<3; i++)
    fFieldComponents[i] = p.fFieldComponents[i];
}

MFMagneticTrap& MFMagneticTrap::operator = (const MFMagneticTrap &p)
{
  if (&p == this) return *this;
  for (G4int i=0; i<3; i++)
    fFieldComponents[i] = p.fFieldComponents[i];
  return *this;
}

void MFMagneticTrap::GetFieldValue (const G4double [4],
				       G4double *B  ) const 
{
  B[0]= fFieldComponents[0] ;
  B[1]= fFieldComponents[1] ;
  B[2]= fFieldComponents[2] ;
}

G4ThreeVector MFMagneticTrap::GetConstantFieldValue() const
{
  G4ThreeVector B(fFieldComponents[0],
		  fFieldComponents[1],
		  fFieldComponents[2]);
  return B;
}
