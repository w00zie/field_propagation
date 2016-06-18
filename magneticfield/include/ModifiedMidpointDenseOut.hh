
/* Modified midpoint which stores derivatives and state at dt/2
 * in some external storage for later usage in dense output calculation
 * This Stepper is for use in Bulirsch Stoer only.
 */

#include <cstring>
#include <vector>
#include <array>

#include "G4Types.hh"
#include "G4FieldTrack.hh"
#include "G4EquationOfMotion.hh"

typedef std::array<G4double,G4FieldTrack::ncompSVEC> state_type;
typedef std::vector<state_type> state_vector_type;

class ModifiedMidpointDenseOut
{
public :
    ModifiedMidpointDenseOut(G4EquationOfMotion* Equation,
                             unsigned int nvar = 6,
                             unsigned int steps = 2):
        Equation_Rhs(Equation),
        fnvar(nvar),
        fsteps(steps)
    {
    }
    ~ModifiedMidpointDenseOut(){
    }

    /*
     * performs a modified midpoint step with fsteps intermediate points
     * stores approximation for x(t+dt/2) in x_mp and all evaluated function results in derivs
     *
     */

    void do_step(const state_type& xIn, const state_type& dxdtIn,
                 G4double t, state_type& xOut,G4double dt, state_type x_mp , state_vector_type& derivs)
    {

        const G4double h = dt / fsteps;
        const G4double h2 = 2 * h;

        G4double th = t + h;

        // x1 = xIn + h*dxdt
        for (unsigned int i = 0; i < fnvar; ++i){
            x1[i] = xIn[i] + h*dxdtIn[i];
        }

        // result of first step already gives approximation at the center of the interval
        if(fsteps == 2)
            x_mp = x1;

        Equation_Rhs->RightHandSide(x1.data(), derivs[0].data()/*, th*/);

        x0 = xIn;

        // general step
        //tmp = x1; x1 = x0 + h2*dxdt; x0 = tmp
        for (unsigned int i = 1; i < fsteps; ++i){
            tmp = x1;
            for (unsigned int j = 0; j < fnvar; ++j){
                x1[j] = x0[j] + h2*dxdt[j];
            }
            x0 = tmp;

            // save approximation at the center of the interval
            if(i == fsteps/2 - 1 )
                x_mp = x1;

            th += h;
            Equation_Rhs->RightHandSide(x1.data(), derivs[i].data()/*, th*/);
        }

        // last step
        // xOut = 0.5*(x0 + x1 + h*dxdt)
        for (unsigned int i = 0; i < fnvar; ++i){
            xOut[i] = 0.5*(x0[i] + x1[i] + h*derivs[fsteps-1][i]);
        }

     }


    void set_steps( unsigned short steps )
    {   fsteps = steps; }


    unsigned short steps() const
    {   return fsteps; }

private:

    G4EquationOfMotion* Equation_Rhs;
    unsigned int fnvar;
    unsigned int fsteps;

    state_type x0;
    state_type x1;
    state_type dxdt;
    state_type tmp;

};
