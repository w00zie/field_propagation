/*
 * VernerRK78.hh
 *
 *  Created on: 8-Jun-2015
 *      Author: hackabot
 */

#ifndef VERNER_RK78_H
#define VERNER_RK78_H

#include "G4MagIntegratorStepper.hh"

class VernerRK78 : public G4MagIntegratorStepper{
    
    
public:
    //constructor
    VernerRK78( G4EquationOfMotion *EqRhs,
               G4int numberOfVariables = 6,
               G4bool primary= true ) ;
    
    //destructor
    ~VernerRK78() ;
    
    //Stepper
    void Stepper( const G4double y[],
                 const G4double dydx[],
                 G4double h,
                 G4double yout[],
                 G4double yerr[] ) ;
    
    G4double  DistChord()   const;
    G4int IntegratorOrder() const { return 7; }
    
    VernerRK78(const VernerRK78&);
    VernerRK78& operator=(const VernerRK78&);
    
    
private:
    
	   G4double *ak2, *ak3, *ak4, *ak5, *ak6, *ak7, *ak8, *ak9, *ak10, *ak11, *ak12, *ak13,        // for storing intermediate 'k' values in stepper
    *yTemp, *yIn;
    
    G4double fLastStepLength;
    G4double *fLastInitialVector, *fLastFinalVector,
    *fLastDyDx, *fMidVector, *fMidError;
    // for DistChord calculations
    
    VernerRK78* fAuxStepper;
    
    //	G4int No_of_vars;
    //	G4double hinit, tinit, tmax, *yinit;
    //	double hmax, hmin, safe_const, err0, Step_factor;
    //	void (*derivs)(double, double *, double *);
    
    
};

#endif /* VernerRK78 */

