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
//
// $Id: NTSTGradientField.hh,v 1.2 2007-10-26 09:51:28 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// Class NTSTGradientField is derived from class G4QuadrupoleMagField
//
// Class description:
//
// Class for creation of quadrupole magnetic field
//   fGradient - is the gradient value for quadrupole magnetic lense.
// Then the magnetic field components are:
//   Bx = B[0] = fGradient*X ,
//   By = B[1] = -fGradient*Y ,
//   Bz = B[2] = 1.5*tesla(Babar Solenoid)
// Here X,Y,Z are the coordinates of a space point of interest.
// -------------------------------------------------------------------

#ifndef NTSTGRADIENTFIELD_HH
#define NTSTGRADIENTFIELD_HH

#include "G4MagneticField.hh"

class NTSTGradientField : public G4MagneticField
{
  public: // with description

    NTSTGradientField(G4double pGradient);
   ~NTSTGradientField();

    void GetFieldValue(const G4double yTrack[],
                             G4double B[]     ) const;
  //without description

  G4int GetCount() const {return count;}
  void ClearCount() {count = 0;}

  protected:
  mutable G4int count;

  private:

    G4double fGradient;
};

#endif
