#include "SynchronousMachine.hxx"
#include "SynchronousMachineDQ0.hxx"
#include "Sensor.hxx"
#include "Actuator.hxx"

using namespace cys;
using std::string;

struct TurbineHouse
{
  string name;
  power::SynchronousMachine m;
  Sensor ws; Actuator mta; //Rotor Control
  Sensor eas; Actuator fca; //Field Control

  TurbineHouse(string name, 
      ulong a_terminal_voltage_tag, ulong field_current_tag, string f_ctl,
      ulong rotor_velocity_tag, ulong mechanical_torque_tag, string w_ctl
  )
  : name{name},
    m{name+".m", 
      0.074, 0.047, 0.047, 
      0.002, 0.002, 0.002, 
      3.3e-3, 1.1e-5, 
      0.2e-3, 0.15e-5,
      0.2e-3, 0.17e-3, 0.01e-3,
      0.015e-3, 0.01e-3,
      0.025e-3, 0.02e-3
    },

    //Rotor Control
    ws{name+".ws", m.w, 1e3_ul, rotor_velocity_tag, w_ctl},
    mta{name+".mta", m.t_m, {-50000, 1.47e6}, {-50000, 50000}, mechanical_torque_tag},

    //Field Control
    eas{name+".eas", m.e_a, 1e6_ul, a_terminal_voltage_tag, f_ctl},
    fca{name+".fca", m.e_fd, {0, 1000}, {0, 1e-6}, field_current_tag}

  {}
};

int main()
{
  TurbineHouse th{
    "myhouse", 
      1001, 1002, "f_ctl",
      1003, 1004, "w_ctl"
  };

  return Sim::get().run(0, 5, 1e-3);
}
