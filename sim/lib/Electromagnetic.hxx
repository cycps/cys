#ifndef CYS_EM
#define CYS_EM

#include "Cypress.hxx"

using std::string;

namespace cys { namespace em {

struct MagneticCore : public Object
{
  // Variables
  Var 
    mmf,  //magnetomotive force
    flux, //magnetic flux
    i;    //coil current

  // Constants
  const realtype 
    L, //length (mean path of integration)
    N, //coil turns
    A, //cross-sectional area
    U, //relative permiativity
    R, //reluctance
    P; //permiance

  MagneticCore(string name, realtype L, realtype N, realtype A, realtype U) 
    : Object(2, name), 
      L{L}, N{N}, A{A}, U{U},
      R{L/U*A},
      P{1/R}
  {
    label(mmf, "mmf");
    label(flux, "flux");
    label(i, "i");
  }

  void resid() override
  {
    r(0) = mmf - N*i;
    r(1) = flux - (mmf * (1/R));
  }
};

}}

#endif
