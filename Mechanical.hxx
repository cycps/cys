#ifndef CYS_MECH
#define CYS_MECH

#include <cmath>
#include "Cypress.hxx"

using std::vector;
using std::string;

namespace cys { namespace mech {

struct Rotor : public Object
{
  Var w, tau, theta;
  realtype H;

  Rotor(string name, realtype H): Object(2, name), H{H} 
  {
    label(w,"w");
    label(tau,"tau");
    label(theta,"theta");
  }

  void resid() override
  {
    r(0) = d(w) - (tau - H*std::pow(w, 2.0));
    r(1) = d(theta) - w;
  }
  
};

}}

#endif
