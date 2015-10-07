#include <iostream>

#include "Actuator.hxx"
#include <cmath>

using namespace cys;
using std::string;
using std::vector;
using std::lock_guard;
using std::mutex;

Actuator::Actuator(string name, Var v, Limits s_limit, Limits d_limit, 
    unsigned long id_tag) 
  : Object{1, name}, v{v}, s_limit(s_limit), d_limit(d_limit), id_tag{id_tag}
{
  Sim::get().actuationServer.enlist(*this);
}

void Actuator::actuate(realtype x)
{
  lock_guard<mutex> lk{Sim::get().mtx};
  clamp(x);
  p = x;
}

void Actuator::clamp(realtype &x)
{
  Sim::get().lg << Sim::get().t << " : _x = " << x << std::endl;

  if(x-p < d_limit[0]) x = p - d_limit[0];
  if(x-p > d_limit[1]) x = p + d_limit[1];
  if(x < s_limit[0]) x = s_limit[0];
  if(x > s_limit[1]) x = s_limit[1];

  Sim::get().lg << Sim::get().t << " : x_ = " << x << std::endl;
}

void Actuator::resid()
{
  r(0) = p - d(v);
}

