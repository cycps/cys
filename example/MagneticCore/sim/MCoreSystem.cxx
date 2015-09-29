#include "Electromagnetic.hxx"
#include "Sensor.hxx"
#include "Actuator.hxx"

using namespace cys;
using std::string;

struct MCoreUnit
{
  string name;
  em::MagneticCore c;
  Actuator a;
  Sensor s;

  MCoreUnit(string name, unsigned long control_tag, string target)
    : name{name},
      c{name+".c", 47000},
      a{name+".a", c.i, 30, 0.3, control_tag},
      s{name+".s", c.flux, 1e6_ul, control_tag, target}
  {}
};

int main()
{
  MCoreUnit("mcu", 1001, "corectl");

  return Sim::get().run(0, 2, 1e-6);
}
