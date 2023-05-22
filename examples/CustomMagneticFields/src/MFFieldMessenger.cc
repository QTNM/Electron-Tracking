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
/// \file field/field02/src/F02FieldMessenger.cc
/// \brief Implementation of the F02FieldMessenger class
//
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "MFFieldMessenger.hh"

#include "MFMagneticFieldSetup.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MFFieldMessenger::MFFieldMessenger(MFMagneticFieldSetup* fieldSetup)
 : G4UImessenger(),
   fElFieldSetup(fieldSetup),
   fFieldDir(0),
   fStepperCmd(0),
   fBFieldZCmd(0),
   fBFieldCmd(0),
   fMinStepCmd(0),
   fUpdateCmd(0)
{
  fFieldDir = new G4UIdirectory("/field/");
  fFieldDir->SetGuidance("MF field tracking control.");

  fStepperCmd = new G4UIcmdWithAnInteger("/field/setStepperType",this);
  fStepperCmd->SetGuidance("Select stepper type for magnetic field");
  fStepperCmd->SetParameterName("choice",true);
  fStepperCmd->SetDefaultValue(4);
  fStepperCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fUpdateCmd = new G4UIcmdWithoutParameter("/field/update",this);
  fUpdateCmd->SetGuidance("Update calorimeter geometry.");
  fUpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  fUpdateCmd->SetGuidance("if you changed geometrical value(s).");
  fUpdateCmd->AvailableForStates(G4State_Idle);

  fBFieldZCmd = new G4UIcmdWithADoubleAndUnit("/field/setFieldZ",this);
  fBFieldZCmd->SetGuidance("Define uniform magnetic field.");
  fBFieldZCmd->SetGuidance("Magnetic field will be in Z direction.");
  fBFieldZCmd->SetGuidance("Value of magnetic field has to be given in Tesla");
  fBFieldZCmd->SetParameterName("Bz",false,false);
  fBFieldZCmd->SetDefaultUnit("tesla");
  fStepperCmd->SetDefaultValue(1.0);
  fBFieldZCmd->AvailableForStates(G4State_Idle);
 
  fBFieldCmd = new G4UIcmdWith3VectorAndUnit("/field/setField",this);
  fBFieldCmd->SetGuidance("Define uniform magnetic field.");
  fBFieldCmd->SetGuidance("Value of magnetic field has to be given in Tesla");
  fBFieldCmd->SetParameterName("Bx","By","Bz",false,false);
  fBFieldCmd->SetDefaultUnit("tesla");
  fBFieldCmd->AvailableForStates(G4State_Idle);
 
  fMinStepCmd = new G4UIcmdWithADoubleAndUnit("/field/setMinStep",this);
  fMinStepCmd->SetGuidance("Define minimal step");
  fMinStepCmd->SetParameterName("min step",false,false);
  fMinStepCmd->SetDefaultUnit("mm");
  fMinStepCmd->AvailableForStates(G4State_Idle);

  fTrapCurrentCmd = new G4UIcmdWithADoubleAndUnit("/field/setCurrent",this);
  fTrapCurrentCmd->SetGuidance("Define trapping current");
  fTrapCurrentCmd->SetParameterName("Trap Current",false,false);
  fTrapCurrentCmd->SetDefaultUnit("ampere");
  fTrapCurrentCmd->SetDefaultValue(1.0);
  fTrapCurrentCmd->AvailableForStates(G4State_Idle);

  fTrapRadiusCmd = new G4UIcmdWithADoubleAndUnit("/field/setRadius",this);
  fTrapRadiusCmd->SetGuidance("Define trapping radius");
  fTrapRadiusCmd->SetParameterName("Trap Radius",false,false);
  fTrapRadiusCmd->SetDefaultUnit("mm");
  fTrapRadiusCmd->SetDefaultValue(100.0);
  fTrapRadiusCmd->AvailableForStates(G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MFFieldMessenger::~MFFieldMessenger()
{
  delete fStepperCmd;
  delete fBFieldZCmd;
  delete fBFieldCmd;
  delete fMinStepCmd;
  delete fFieldDir;
  delete fUpdateCmd;
  delete fTrapCurrentCmd;
  delete fTrapRadiusCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MFFieldMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if( command == fStepperCmd )
    fElFieldSetup->SetStepperType(fStepperCmd->GetNewIntValue(newValue));
  if( command == fUpdateCmd )
    fElFieldSetup->UpdateIntegrator();
  if( command == fBFieldZCmd )
    fElFieldSetup->SetFieldZValue(fBFieldZCmd->GetNewDoubleValue(newValue));
  if( command == fBFieldCmd )
    fElFieldSetup->SetFieldValue(fBFieldCmd->GetNew3VectorValue(newValue));
  if( command == fMinStepCmd )
    fElFieldSetup->SetMinStep(fMinStepCmd->GetNewDoubleValue(newValue));
  if( command == fTrapCurrentCmd )
    fElFieldSetup->SetTrapCurrent(fTrapCurrentCmd->GetNewDoubleValue(newValue));
  if( command == fTrapRadiusCmd )
    fElFieldSetup->SetTrapRadius(fTrapRadiusCmd->GetNewDoubleValue(newValue));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
