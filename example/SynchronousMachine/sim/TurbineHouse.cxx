#include "SynchronousMachine.hxx"
#include "Sensor.hxx"
#include "Actuator.hxx"

using namespace cys;
using std::string;

struct TurbineHouse
{
  string name;
  power::SynchronousMachine m;
  Sensor eas, ws;
  Actuator fca, mta;

  TurbineHouse(string name, 
      unsigned long field_current_tag, unsigned long mechanical_torque_tag, 
      unsigned long a_terminal_voltage_tag, unsigned long rotor_velocity_tag,
      string target
  )
  : name{name},
    m{name+".m", 
      0.074, 0.047, 0.047, 
      0.002, 0.002, 0.002, 
      3.3e-3, 1.1e-5, 0.2e-3, 0.15e-5,
      0.2e-3, 0.17e-3, 0.01e-3,
      0.015e-3, 0.01e-3,
      0.025e-3, 0.02e-3
    },
    eas{name+".eas", m.e_a, 1e6_ul, a_terminal_voltage_tag, target},
    ws{name+".ws", m.w, 1e3_ul, rotor_velocity_tag, target},
    fca{name+".fca", m.e_fd, 30, 1, field_current_tag},
    mta{name+".mta", m.t_m, 377, 1, mechanical_torque_tag}
  {}
};

int main()
{
  TurbineHouse th{"myhouse", 1001, 1002, 1003, 1004, "ctl"};

  return Sim::get().run(0, 10, 1e-3);
}
