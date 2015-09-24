#ifndef CYS_MECH
#define CYS_MECH

#include <cmath>
#include "Cypress.hxx"

namespace cys { namespace mech {

struct Rotor : public Object
{
  Var w, tau, theta;
  realtype H;

  Rotor(realtype H): Object(2), H{H} {}

  void Resid() override
  {
    r(0) = d(w) - tau + H*std::pow(w, 2);
    r(1) = d(theta) - w;
  }
};

}}

#endif
