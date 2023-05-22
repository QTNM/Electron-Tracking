//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file field/field02/include/F02MagneticFieldSetup.hh
/// \brief Definition of the F02MagneticFieldSetup class
//
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef MFMagneticFieldSetup_h
#define MFMagneticFieldSetup_h 1

#include "G4MagneticField.hh"
#include "G4ThreeVector.hh"
#include "MFMagneticTrap.hh"

class G4FieldManager;
class G4ChordFinder;
class G4Mag_EqRhs;
class G4MagIntegratorStepper;
class G4MagInt_Driver;
class MFFieldMessenger;

/// A class for control of the Magnetic Field of the detector.
///     The field for this case is uniform.
///
/// The field for this case is uniform.
/// It is simply a 'setup' class that creates the field and necessary
/// other parts

class MFMagneticFieldSetup
{
public:
  MFMagneticFieldSetup(G4ThreeVector);  //  The value of the field
  MFMagneticFieldSetup();               //  A zero field - true value set later

  virtual ~MFMagneticFieldSetup();

   // Methods to set parameters or select 
  void SetStepperType( G4int i) { fStepperType = i ; CreateStepper(); }

  void SetMinStep(G4double s) { fMinStep = s ; }

  void SetFieldValue(G4ThreeVector fieldVector);
  void SetFieldZValue(G4double      fieldValue);
  void SetTrapCurrent(G4double fTrapCurrent);
  G4ThreeVector GetConstantFieldValue();
   // Set/Get Field strength in Geant4 units

  void UpdateIntegrator();
   // Prepare all the classes required for tracking - from stepper 
   //    to Chord-Finder
   //   NOTE:  field and equation must have been created before calling this.
   
protected:

  // Find the global Field Manager

  G4FieldManager*         GetGlobalFieldManager();

  void CreateStepper();
   // Implementation method - should not be exposed

private:
  G4double                fMinStep;
  G4bool                  fVerbose;

  G4FieldManager*         fFieldManager;

  G4ChordFinder*          fChordFinder;

  G4Mag_EqRhs*            fEquation;

  MFMagneticTrap*         fEMfield;
 
  G4ThreeVector           fElFieldValue;

  G4MagIntegratorStepper* fStepper;
  G4MagInt_Driver*        fIntgrDriver;

  G4int                   fStepperType;

   
  MFFieldMessenger*       fFieldMessenger;

};

#endif
