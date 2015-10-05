#ifndef CYS_POWER
#define CYS_POWER

#include "Cypress.hxx"

using std::string;

namespace cys { namespace power {

/*==============================================================================
 * SynchronousMachine
 * ..................
 *
 * The classic 3-phase synchronous machine model as presented in Kundur.
 *
 * ===========================================================================*/
struct SynchronousMachine : public Object
{
  // Variables: (21) +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Var
    f_a, f_b, f_c,        //phase flux linkages
    e_a, e_b, e_c,        //stator phase to neutral voltages
    i_a, i_b, i_c,        //stator phase currents
    i_fd, i_kd, i_kq,     //field and amortisseur circuit currents
    l_aa, l_bb, l_cc,     //self inductances of stator windings
    l_afd, l_akd, l_akq,  //mutual inductances between stator and rotor windings
    l_ffd, l_kkd, l_kkq;  //self inductances of rotor circuits

  // Controlled Variables: (1) +++++++++++++++++++++++++++++++++++++++++++++++++
  Var
    e_fd;                 //field voltage

  // Constants +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  const realtype
    R_fd, R_kd, R_kq,   //rotor circuit resistances
    R_a, R_b, R_c;      //phase armature resistance

  SynchronousMachine(string name, 
      realtype R_fd, realtype R_kd, realtype R_kq,
      realtype R_a, realtype R_b, realtype R_c
  )
  : Object(21, name),
    R_fd{R_fd}, R_kd{R_kd}, R_kq{R_kq},
    R_a{R_a}, R_b{R_b}, R_c{R_c}
  {

  }

  void resid() override
  {
    r(0) = e_a - (d(f_a) - R_a*i_a);
    r(1) = e_b - (d(f_b) - R_b*i_b);
    r(2) = e_c - (d(f_c) - R_c*i_c);
    //...
    //r(20) = ...
  }

};

}}

#endif
