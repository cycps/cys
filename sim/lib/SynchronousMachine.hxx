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
  // Variables: (27) +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Var
    f_a, f_b, f_c,        //phase flux linkages
    f_fd, f_kd, f_kq,     //rotor flux linkage
    e_a, e_b, e_c,        //stator phase to neutral voltages
    i_a, i_b, i_c,        //stator phase currents
    i_fd, i_kd, i_kq,     //field and amortisseur circuit currents
    l_aa, l_bb, l_cc,     //self inductances of stator windings
    l_ab, l_bc, l_ca,     //mutual inductances of stator windings
    l_afd, l_akd, l_akq,  //mutual inductances between stator and rotor windings
    theta,                //rotor position relative to the a-phase axis
    w,                    //rotor angular velocity
    t_e;                  //electrical and mechanical torque on the rotor

  // Controlled Variables: (2) +++++++++++++++++++++++++++++++++++++++++++++++++
  Var
    e_fd,                 //field voltage
    t_m;                  //mechanical torque

  // Constants +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  const realtype
    R_fd, R_kd, R_kq, //rotor circuit resistances
    R_a, R_b, R_c,    //phase armature resistance
    L_aa0, L_aa2,     //self air gap inductance, fundamental & second harmonic
    L_ab0, L_ab2,     //mutual air gap inductance, fundamental & second harmonic
    L_afd, L_akd, L_akq, //rotor-stator mutual inductances
    L_ffd,            //field self inductance armatisseur inductances
    L_fkd,            //field-amortisseur mutual inductance
    L_kkq, L_kkd;     //amortisseur self inductances

  SynchronousMachine(string name, 
      realtype R_fd, realtype R_kd, realtype R_kq,
      realtype R_a, realtype R_b, realtype R_c,
      realtype L_aa0, realtype L_aa2,
      realtype L_ab0, realtype L_ab2,
      realtype L_afd, realtype L_akd, realtype L_akq,
      realtype L_ffd, realtype L_fkd,
      realtype L_kkq, realtype L_kkd
  )
  : Object(27, name),
    R_fd{R_fd}, R_kd{R_kd}, R_kq{R_kq},
    R_a{R_a}, R_b{R_b}, R_c{R_c},
    L_aa0{L_aa0}, L_aa2{L_aa2},
    L_ab0{L_ab0}, L_ab2{L_ab2},
    L_afd{L_afd}, L_akd{L_akd}, L_akq{L_akq},
    L_ffd{L_ffd}, L_fkd{L_fkd},
    L_kkq{L_kkq}, L_kkd{L_kkd}
  {
    makeLabels();
  }

  //because the machine inductances are nonzero at t=0 we need to
  //initialize them
  void init() override
  {
    //Phase self inductances
    l_aa = L_aa0 + L_aa2*cos(2*theta);
    l_bb = L_aa0 + L_aa2*cos(2*(theta - 2*PI/3));
    l_cc = L_aa0 + L_aa2*cos(2*(theta + 2*PI/3));

    //Phase mutual inductances
    l_ab = -L_ab0 - L_ab2*cos(2*theta + PI/3);
    l_bc = -L_ab0 - L_ab2*cos(2*theta - PI);
    l_ca = -L_ab0 - L_ab2*cos(2*theta - PI/3);
    
    l_afd = L_afd*cos(theta);
    l_akd = L_akd*cos(theta);
    l_akq = -L_akq*sin(theta);
  }

  void resid() override
  {
    //Phase terminal voltages
    r(0) = e_a - (d(f_a) - R_a*i_a);
    r(1) = e_b - (d(f_b) - R_b*i_b);
    r(2) = e_c - (d(f_c) - R_c*i_c);

    //Phase flux linkages
    r(3) = f_a - (
      - l_aa*i_a 
      - l_ab*i_b 
      - l_ca*i_c 
      + l_afd*i_fd 
      + l_akd*i_kd 
      + l_akq*i_kq
    );
    
    r(4) = f_b - (
      - l_bb*i_b 
      - l_ab*i_a 
      - l_bc*i_c 
      + l_afd*i_fd 
      + l_akd*i_kd 
      + l_akq*i_kq
    );
    
    r(5) = f_c - (
      - l_cc*i_c 
      - l_ca*i_a 
      - l_bc*i_b 
      + l_afd*i_fd 
      + l_akd*i_kd 
      + l_akq*i_kq
    );

    //Phase self inductances
    r(6) = l_aa - (
      L_aa0 + L_aa2*cos(2*theta)
    );

    r(7) = l_bb - (
      L_aa0 + L_aa2*cos(2*(theta - 2*PI/3))
    );
    
    r(8) = l_cc - (
      L_aa0 + L_aa2*cos(2*(theta + 2*PI/3))
    );
    

    //Phase mutual inductances
    r(9) = l_ab - (
      -L_ab0 - L_ab2*cos(2*theta + PI/3)
    );

    r(10) = l_bc - (
      -L_ab0 - L_ab2*cos(2*theta - PI)
    );
    
    r(11) = l_ca - (
      -L_ab0 - L_ab2*cos(2*theta - PI/3)
    );

    //Rotor-Stator mutual inductance
    r(12) = l_afd - (
      L_afd*cos(theta)
    );

    r(13) = l_akd - (
      L_akd*cos(theta)
    );

    r(14) = l_akq - (
      -L_akq*sin(theta)
    );

    //Rotor voltage equations
    r(15) = e_fd - (
      d(f_fd) + R_fd*i_fd
    );

    r(16) = - (
      d(f_kd) + R_kd*i_kd
    );
    
    r(17) = - (
      d(f_kq) + R_kq*i_kq 
    );

    //Rotor flux linkage equations
    r(18) = f_fd - (
      L_ffd*i_fd 
      + L_fkd*i_kd 
      - L_afd * ( 
          i_a*cos(theta) + i_b*cos(theta - 2*PI/3) + i_c*cos(theta + 2*PI/3)
        )
    );

    r(19) = f_kd - (
      L_fkd*i_fd
      + L_kkd*i_kd
      - L_akd * (
        i_a*cos(theta) + i_b*cos(theta - 2*PI/3) + i_c*cos(theta + 2*PI/3)
      )
    );

    r(20) = f_kq - (
      L_kkq*i_kq 
      + L_akq * (
        i_a*sin(theta) + i_b*sin(theta - 2*PI/3) + i_c*sin(theta + 2*PI/3) 
      )
    );

    //electrical torque
    r(21) = t_e - (
        f_a*i_a + f_b*i_b + f_c*i_b
    );

    //rotor motion
    r(22) = d(theta) - (
      t_m - t_e
    );

    r(23) = w - d(theta);

    //check down below
    //^^^^^^^^^^^^^^^^
    
    //field current
    r(24) = e_fd - (
        i_fd * R_fd
    );

    //amortisseur currents
    r(25) = d(f_kd) - (
        i_kd * R_kd
    );

    r(26) = d(f_kq) - (
        i_kq * R_kq
    );

  }

  //yeah this is gross, we need a better / more automatic way of labeling 
  //variables
  void makeLabels()
  {
    label({
       {f_a,    "f_a"},
       {f_b,    "f_b"}, 
       {f_c,    "f_c"},
       {e_a,    "e_a"},
       {e_b,    "e_b"},
       {e_c,    "e_c"},
       {i_a,    "i_a"},
       {i_b,    "i_b"},
       {i_c,    "i_c"},
       {i_fd,   "i_fd"},
       {i_kd,   "i_kd"},
       {i_kq,   "i_kq"},
       {l_aa,   "l_aa"},
       {l_bb,   "l_bb"},
       {l_cc,   "l_cc"},
       {l_ab,   "l_ab"},
       {l_bc,   "l_bc"},
       {l_ca,   "l_ca"},
       {l_afd,  "l_afd"},
       {l_akd,  "l_akd"},
       {l_akq,  "l_akq"},
    });
  }

};

}}

#endif
