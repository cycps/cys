#include "SynchronousMachine.hxx"
#include "Sensor.hxx"
#include "Actuator.hxx"

using namespace cys;
using std::string;

struct TurbineHouse
{
  string name;
  power::SynchronousMachine m;
  Sensor s;
  Actuator a;

  TurbineHouse(string name, unsigned long control_tag, string target)
    : name{name},
      m{name+".m", 
        0.074, 0.047, 0.047, 
        0.002, 0.002, 0.002, 
        3.3e-3, 1.1e-5, 0.2e-3, 0.15e-5,
        0.2e-3, 0.17e-3, 0.01e-3,
        0.015e-3, 0.01e-3,
        0.025e-3, 0.02e-3
      },
      s{name+".s", m.e_a, 1e6_ul, control_tag, target},
      a{name+".a", m.e_fd, 100, 1, control_tag}
  {}
};

int main()
{
  TurbineHouse th{"myhouse", 1001, "ctl"};

  return Sim::get().run(0, 10, 1e-3);
}
