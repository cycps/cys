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
    std::cout << "w=" << (realtype)w << std::endl;
    std::cout << "tau=" << (realtype)tau << std::endl;
    std::cout << "theta=" << (realtype)theta << std::endl;
    r(0) = d(w) - (tau - H*std::pow(w, 2.0));
    r(1) = d(theta) - w;
    std::cout << "r(0)=" << r(0) << std::endl;
    std::cout << "r(1)=" << r(1) << std::endl;
    std::cout << "r(2)=" << r(2) << std::endl;
  }
};

}}

#endif
