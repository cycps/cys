#include "Mechanical.hxx"
#include "Sensor.hxx"
#include "Actuator.hxx"

using namespace cys;

int main()
{
  mech::Rotor r{47};
  Actuator a{r.tau, 10, 0.5, 1001};
  Sensor s{r.w, 30, 1001, "localhost"};

  return Sim::get().run(0, 10, 1e-6);
}

