#ifndef CYS_EM
#define CYS_EM

#include "Cypress.hxx"

using std::string;

namespace cys { namespace em {

struct MagneticCore : public Object
{
  Var mmf, flux, i;
  realtype R;

  MagneticCore(string name, realtype R) : Object(2, name), R{R}
  {
    label(flux, "flux");
    label(i, "i");
  }

  void resid() override
  {
    r(0) = flux - (mmf * (1/R));
    //r(1) =  ...
  }
};

}}

#endif
