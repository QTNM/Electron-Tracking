#ifndef MFMagneticTrap_h
#define MFMagneticTrap_h 1

#include "G4Types.hh"
#include "G4ThreeVector.hh"
#include "G4MagneticField.hh"

#ifdef HAVE_BOOST
#include <boost/math/special_functions/ellint_1.hpp>
#include <boost/math/special_functions/ellint_2.hpp>
#else
#define __STDCPP_WANT_MATH_SPEC_FUNCS__ 1
#endif

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
  G4double radius_;
  G4double current_;
  G4double b_central_;

  void EvaluateCoil(const G4double yIn[7], const G4double zcoil, G4double field[3]) const;

};
#endif
