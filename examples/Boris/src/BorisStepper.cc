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
// G4ExactHelixStepper implementation
//
// Author: J.Apostolakis, 28.01.2005.
//         Implementation adapted from ExplicitEuler by W.Wander
// -------------------------------------------------------------------

#include "G4PhysicalConstants.hh"
#include "G4ThreeVector.hh"
#include "G4LineSection.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "BorisStepper.hh"

const G4double BorisStepper::fUnitConstant = 0.299792458*(GeV/(tesla*m));

BorisStepper::BorisStepper(G4Mag_EqRhs* EqRhs)
  : G4MagHelicalStepper(EqRhs),
    fBfieldValue(DBL_MAX, DBL_MAX, DBL_MAX),
    fPtrMagEqOfMot(EqRhs)
{
}

BorisStepper::~BorisStepper()
{
}

// ---------------------------------------------------------------------------

void
BorisStepper::Stepper( const G4double yInput[],
                              const G4double*,
                                    G4double hstep,
                                    G4double yOut[],
                                    G4double yErr[] )
{
  const G4int nvar = 6;

  G4int i;
  G4ThreeVector Bfld_value;
  
  G4ThreeVector x_init = G4ThreeVector( yInput[0], yInput[1], yInput[2]);
  const G4double* pIn = yInput+3;
  G4ThreeVector initVelocity = G4ThreeVector( pIn[0], pIn[1], pIn[2]);
  G4double      velocityVal = initVelocity.mag();
  G4ThreeVector initTangent = (1.0/velocityVal) * initVelocity;

  G4ThreeVector x_half;

  // Half time-step motion
  x_half = x_init + initTangent * hstep * 0.5;

  // Evaluate the field - TODO FIX ME. This should use half timestep position
  MagFieldEvaluate(yInput, Bfld_value);

  G4ThreeVector endTangent = RotateVelocity(Bfld_value, hstep, initVelocity);

  // Half time-step motion
  yOut[0]   = x_half[0] + endTangent.x() * hstep * 0.5;
  yOut[1]   = x_half[1] + endTangent.y() * hstep * 0.5;
  yOut[2]   = x_half[2] + endTangent.z() * hstep * 0.5;
  yOut[3] = velocityVal * endTangent.x();
  yOut[4] = velocityVal * endTangent.y();
  yOut[5] = velocityVal * endTangent.z();

  // Store and/or calculate parameters for chord distance - Needs doing

  // G4double ptan=velocityVal*B_v_P;

  // G4double particleCharge = fPtrMagEqOfMot->FCof() / (eplus*c_light);
  // R_Helix =std::abs( ptan/(fUnitConstant  * particleCharge*Bmag));

  // SetAngCurve(std::abs(Theta));
  // SetCurve(std::abs(R));
  // SetRadHelix(R_Helix);


  // We are assuming a constant field: helix is exact
  //
  for(i=0; i<nvar; ++i)
  {
    yErr[i] = 0.0 ;
  }

  fBfieldValue = Bfld_value;
}

G4ThreeVector
BorisStepper::RotateVelocity( G4ThreeVector Bfld_value, G4double hstep, G4ThreeVector velocity)
{
  G4double      velocityVal = velocity.mag();
  G4ThreeVector initTangent = (1.0/velocityVal) * velocity;
  G4ThreeVector Bnorm;
  G4double R_1, Theta;

  G4double Bmag = Bfld_value.mag();

  // Now perform the rotation - need angle
  Bnorm = (1.0/Bmag)*Bfld_value;
  R_1 = GetInverseCurve(velocityVal,Bmag);
  Theta   = R_1 * hstep;

  // Rotation - re-evaluate tangent
  G4ThreeVector t = std::tan(0.5*Theta) * Bnorm;
  G4ThreeVector u_prime = initTangent + initTangent.cross(t);
  G4ThreeVector endTangent = initTangent + 2.0 / (1.0 + t.dot(t)) * u_prime.cross(t);

  return endTangent;
}

// ---------------------------------------------------------------------------

void
BorisStepper::DumbStepper( const G4double yIn[],
                                        G4ThreeVector Bfld,
                                        G4double h,
                                        G4double yOut[])
{
  // Assuming a constant field: solution is a helix

  AdvanceHelix(yIn, Bfld, h, yOut);

  G4Exception("BorisStepper::DumbStepper",
              "GeomField0002", FatalException,
              "Should not be called. Stepper must do all the work." );
}


// ---------------------------------------------------------------------------

G4double
BorisStepper::DistChord() const
{
  // Implementation : must check whether h/R >  pi  !!
  //   If( h/R <  pi)   DistChord=h/2*std::tan(Ang_curve/4)
  //   Else             DistChord=R_helix

  G4double distChord;
  G4double Ang_curve=GetAngCurve();

  if (Ang_curve<=pi)
  {
    distChord=GetRadHelix()*(1-std::cos(0.5*Ang_curve));
  }
  else if(Ang_curve<twopi)
  {
    distChord=GetRadHelix()*(1+std::cos(0.5*(twopi-Ang_curve)));
  }
  else
  {
    distChord=2.*GetRadHelix();
  }

  return distChord;
}

// ---------------------------------------------------------------------------

G4int
BorisStepper::IntegratorOrder() const
{
  return 1;
}
