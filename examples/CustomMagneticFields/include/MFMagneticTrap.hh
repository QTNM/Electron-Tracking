#ifndef MFMagneticTrap_h
#define MFMagneticTrap_h 1

#include "G4Types.hh"
#include "G4ThreeVector.hh"
#include "G4MagneticField.hh"

class MFMagneticTrap : public G4MagneticField
{
public:  // with description
  
  MFMagneticTrap(const G4ThreeVector& FieldVector );

  virtual ~MFMagneticTrap() ;

  MFMagneticTrap(const MFMagneticTrap &p);
  MFMagneticTrap& operator = (const MFMagneticTrap &p);

  virtual void GetFieldValue(const G4double yTrack[4],
			     G4double *MagField) const ;

  void SetFieldValue(const G4ThreeVector& newFieldValue);

  G4ThreeVector GetConstantFieldValue() const;

private:

  G4double fFieldComponents[3] ;
};
#endif
