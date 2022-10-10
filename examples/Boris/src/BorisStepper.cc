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

  MagFieldEvaluate(yInput, Bfld_value);

  // Begin AdvanceHelix Routine
  //AdvanceHelix(yInput, Bfld_value, hstep, yOut);

  const G4double approc_limit = 0.005;
  G4ThreeVector  Bnorm, B_x_P, vperp, vpar;

  G4double B_d_P;
  G4double B_v_P;
  G4double Theta;
  G4double R_1;
  G4double R_Helix;
  G4double CosT, SinT;
  G4ThreeVector positionMove, endTangent;

  G4double Bmag = Bfld_value.mag();
  const G4double* pIn = yInput+3;
  G4ThreeVector initVelocity = G4ThreeVector( pIn[0], pIn[1], pIn[2]);
  G4double      velocityVal = initVelocity.mag();
  G4ThreeVector initTangent = (1.0/velocityVal) * initVelocity;

  R_1 = GetInverseCurve(velocityVal,Bmag);

  Bnorm = (1.0/Bmag)*Bfld_value;

  // calculate the direction of the force

  B_x_P = Bnorm.cross(initTangent);

  // parallel and perp vectors

  B_d_P = Bnorm.dot(initTangent); // this is the fraction of P parallel to B

  vpar = B_d_P * Bnorm;       // the component parallel      to B
  vperp= initTangent - vpar;  // the component perpendicular to B

  B_v_P  = std::sqrt( 1 - B_d_P * B_d_P); // Fraction of P perp to B

  // calculate  the stepping angle

  Theta   = R_1 * hstep; // * B_v_P;
  // Trigonometrix

  SinT     = std::sin(Theta);
  CosT     = std::cos(Theta);

  // the actual "rotation"

  G4double R = 1.0 / R_1;

  positionMove  = R * ( SinT * vperp + (1-CosT) * B_x_P) + hstep * vpar;
  endTangent    = CosT * vperp + SinT * B_x_P + vpar;

  // Store the resulting position and tangent

  yOut[0]   = yInput[0] + positionMove.x();
  yOut[1]   = yInput[1] + positionMove.y();
  yOut[2]   = yInput[2] + positionMove.z();
  yOut[3] = velocityVal * endTangent.x();
  yOut[4] = velocityVal * endTangent.y();
  yOut[5] = velocityVal * endTangent.z();

  // Store and/or calculate parameters for chord distance

  G4double ptan=velocityVal*B_v_P;

  G4double particleCharge = fPtrMagEqOfMot->FCof() / (eplus*c_light);
  R_Helix =std::abs( ptan/(fUnitConstant  * particleCharge*Bmag));

  SetAngCurve(std::abs(Theta));
  SetCurve(std::abs(R));
  SetRadHelix(R_Helix);

  //end AdvanceHelix Routine

  // We are assuming a constant field: helix is exact
  //
  for(i=0; i<nvar; ++i)
  {
    yErr[i] = 0.0 ;
  }

  fBfieldValue = Bfld_value;
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
