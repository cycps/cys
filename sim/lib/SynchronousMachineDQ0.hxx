#ifndef CYS_SYNCCDQ0
#define CYS_SYNCCDQ0

#include "Cypress.hxx"

namespace cys { namespace power {
/*==============================================================================
 * SynchronousMachineDQ0
 * .....................
 *
 * The classic DQ0 synchronous machine model as presented in Kundur.
 *
 * ===========================================================================*/

struct SynchronousMachineDQ0 : public Object
{
  //Variables (19)
  Var
    e_d, e_q, e_0, 
    f_d, f_q, f_0, f_fd, f_1d, f_1q, f_2q,
    i_d, i_q, i_0, i_fd, i_1d, i_1q, i_2q,
    T_e,
    w_r;
  
  //Controlled Variables
  Var
    e_fd, T_m;

  //Constants
  realtype 
    R_a, R_fd, R_1d, R_1q, R_2q,
    L_ad, L_l, L_aq, L_0, L_ffd, L_f1d, L_11d, L_11q, L_22q,
    H, K_d, w_0;

  void init() override
  {

  }

  void resid() override
  {
    //stator voltage
    r(0) = e_d - (
        d(f_d) - f_q*w_r - R_a*i_d
    );
    r(1) = e_q - (
        d(f_q) + f_d*w_r - R_a*i_q
    );
    r(2) = e_0 - (
        d(f_0) - R_a*i_0
    );

    //rotor voltage
    r(3) = e_fd - (
      d(f_fd) + R_fd*i_fd
    );
    r(4) = - (
      d(f_1d) + R_1d*i_1d
    );
    r(5) = - (
      d(f_1q) + R_1q*i_1q
    );
    r(6) = - (
      d(f_2q) + R_2q*i_2q
    );

    //stator flux linkage
    r(7) = f_d - (
        -(L_ad + L_l)*i_d + L_ad*i_fd + L_ad*i_1d
    );
    r(8) = f_q - (
        -(L_aq + L_l)*i_q + L_aq*i_1q + L_aq*i_2q
    );
    r(9) = f_0 - (
        -L_0*i_0
    );

    //rotor flux linkages
    r(10) = f_fd - (
        L_ffd*i_fd + L_f1d*i_1d - L_ad*i_d
    );
    r(11) = f_1d - (
        L_f1d*i_fd + L_11d*i_1d - L_ad*i_d
    );
    r(12) = f_1q - (
        L_11q*i_1q + L_aq*i_2q - L_aq*i_q
    );
    r(13) = f_2q - (
        L_aq*i_1q + L_22q*i_2q - L_aq*i_q
    );
    r(14) = T_e - (
      f_d*i_q - f_q*i_d
    );

    //swing equation
    r(15) = (2*H/w_0)*d(w_r) - (
        T_m - T_e - K_d*d(w_r)
    );
    

  }
};

}}

#endif
