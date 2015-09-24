#include <iostream>

#include "Actuator.hxx"
#include <cmath>

using namespace cys;

Actuator::Actuator(Var v, realtype s_limit, realtype d_limit, unsigned long id_tag) 
  : Object{1}, v{v}, s_limit{s_limit}, d_limit{d_limit}, id_tag{id_tag}
{
  Sim::get().actuationServer.enlist(*this);
}

void Actuator::actuate(realtype x)
{
  clamp(x);
  p = x;
}

void Actuator::clamp(realtype &x)
{
  if(x > s_limit) x = s_limit;
  if(std::abs(x-p) > d_limit) x = p + d_limit * (x)/std::abs(x);
}

void Actuator::Resid()
{
  r(0) = p - d(v);
}
