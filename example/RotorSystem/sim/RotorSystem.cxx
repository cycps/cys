#include "Mechanical.hxx"
#include "Sensor.hxx"
#include "Actuator.hxx"

using namespace cys;
using std::string;

struct RotorHouse
{
  string name;
  mech::Rotor r;
  Actuator a;
  Sensor s;

  RotorHouse(string name, unsigned long control_tag, string target)
    : name{name},
      r{name+".r", 47},
      a{name+".a", r.tau, {0, 10}, {0, 0.5}, control_tag},
      s{name+".s", r.w, 120, control_tag, target}
  { }
};

int main()
{
  RotorHouse a{"a", 1001, "rca"};
  RotorHouse b{"b", 1002, "rcb"};
  RotorHouse c{"c", 1003, "rcc"};

  return Sim::get().run(0, 2, 1e-3);
}

