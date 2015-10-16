#ifndef CYS_STATOR
#define CYS_STATOR

#include "Cypress.hxx"

namespace cys { namespace power {

struct Stator : public Object
{
  //Variables
  Var
    i_a, i_b, i_c,      //stator phase currents
    h_a, h_b, h_c,      //phase magnetic field intensities
    b_a, b_b, b_c;      //phase flux densities

  const realtype
    I_max,              //peak stator current
    N,                  //turns per stator coil
    U,                  //flux path permittivity
    W_s;                //stator rotational velocity

  Stator(string name, realtype I_max, realtype N, realtype U_r, realtype H_z)
    : Object(9, name), I_max{I_max}, N{N}, U{U_r*U_0}, W_s{deg(360)*H_z}
  {
    makeLabels();
  }

  void init() override
  {
    i_b = I_max*sin(W_s*0 + deg(120));
    i_c = I_max*sin(W_s*0 + deg(240));
    h_b = i_b*N;
    h_c = i_c*N;
    b_b = h_b*U;
    b_c = h_c*U;
  }

  void resid() override
  {
    realtype t = Sim::get().t;
    //stator phase currents
    r(0) = i_a - I_max*sin(W_s*t);
    r(1) = i_b - I_max*sin(W_s*t + deg(120));
    r(2) = i_c - I_max*sin(W_s*t + deg(240));

    //phase magnetic field intensities
    r(3) = h_a - i_a*N;
    r(4) = h_b - i_b*N;
    r(5) = h_c - i_c*N;

    //phase flux densities
    r(6) = b_a - h_a*U;
    r(7) = b_b - h_b*U;
    r(8) = b_c - h_c*U;
  }

  void makeLabels()
  {
    label({
      {i_a, "i_a"},
      {i_b, "i_b"},
      {i_c, "i_c"},
      {h_a, "h_a"},
      {h_b, "h_b"},
      {h_c, "h_c"},
      {b_a, "b_a"},
      {b_b, "b_b"},
      {b_c, "b_c"}
    });
  }
};

}}

#endif
