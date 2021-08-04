// QTNMPysicsList
//
//----------------------------------------------------------------------------
//
// This class provides construction of EM physics for the QTNM project
// following closely the G4EmStandardPhysics_option4 list 
// of standard and low-energy packages and set of 
// the most adavced options allowing precise simulation at low
// and intermediate energies.
// Main difference: removal of electron multiple scattering with 
//                  single electron scattering to account for
//                  very low target material density.
//

#ifndef QTNMPhysicsList_h
#define QTNMPhysicsList_h 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"


class QTNMPhysicsList : public G4VPhysicsConstructor
{
public:

  explicit QTNMPhysicsList(G4int ver=1, const G4String& name="");

  ~QTNMPhysicsList() override;

  void ConstructParticle() override;
  void ConstructProcess() override;

private:
  G4int  verbose;
};


#endif

