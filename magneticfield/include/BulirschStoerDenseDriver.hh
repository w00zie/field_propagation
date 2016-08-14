//
// class BulirschStoerDenseDriver
//
// Class description:
//
// This is a driver class. It uses Bulirsch-Stoer algorithm with
// dense output to integrate ODE.
//
// History:
// - Created. D.Sorokin.
// --------------------------------------------------------------------

#ifndef BulirschStoerDenseDriver_HH
#define BulirschStoerDenseDriver_HH

#include "G4VIntegrationDriver.hh"
#include "BulirschStoerDenseOut.hh"

class BulirschStoerDenseDriver: public G4VIntegrationDriver{

public:
    BulirschStoerDenseDriver(G4double hminimum, G4EquationOfMotion* pequation,
                             G4int numberOfComponents = 6, G4int VerboseLevel = 1);

    ~BulirschStoerDenseDriver();

    BulirschStoerDenseDriver(const BulirschStoerDenseDriver&) = delete;
    BulirschStoerDenseDriver& operator=(const BulirschStoerDenseDriver&) = delete;


    virtual G4bool  AccurateAdvance(G4FieldTrack&  track,G4double stepLen,
                                    G4double eps,G4double hinitial = 0) override final;

    virtual G4bool QuickAdvance(G4FieldTrack& track, const G4double dydx[],
                                G4double hstep,G4double& missDist, 
                                G4double& dyerr ) override final;

    virtual void  OneGoodStep(G4double  y[], const G4double  dydx[],
                              G4double& curveLength, G4double htry,
                              G4double  eps, G4double& hdid,
                              G4double& hnext) override final;

    virtual G4double ComputeNewStepSize(G4double /*dyErr_relative*/, G4double lastStepLength ) override final;


    //dense output methods
    virtual G4bool isDense() const;
    virtual void DoStep(G4FieldTrack& track, G4double hstep, G4double eps) override final;
    virtual void DoInterpolation(G4FieldTrack& track, G4double hstep, G4double eps = 0) override final;

private:

    ModifiedMidpointDenseOut denseMidpoint;
    BulirschStoerDenseOut bulirschStoer;


    G4double yIn[G4FieldTrack::ncompSVEC],
             yMid[G4FieldTrack::ncompSVEC],
             yMid2[G4FieldTrack::ncompSVEC],
             yOut[G4FieldTrack::ncompSVEC],
             yOut2[G4FieldTrack::ncompSVEC],
             yError[G4FieldTrack::ncompSVEC];

    G4double dydxCurrent[G4FieldTrack::ncompSVEC];
    G4double yCurrent[G4FieldTrack::ncompSVEC];

    G4double derivs[2][6][G4FieldTrack::ncompSVEC];
    G4double diffs[4][2][G4FieldTrack::ncompSVEC];

    const G4int interval_sequence[2];
    const G4double fcoeff;

    //for interpolation
    G4double eps_prev;
    G4double fNextStepSize;

};


#endif
