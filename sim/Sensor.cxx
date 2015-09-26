#include <stdexcept>
#include "Sensor.hxx"

using namespace cys;
using std::string;
using std::runtime_error;
  
Sensor::Sensor(string name, Var v, unsigned long rate, unsigned long id_tag, 
    string target) 
  : v{v}, rate{rate}, id_tag{id_tag}, nxt{static_cast<realtype>(rate)}, 
    target{target}, name{name}
{
  bzero(&out_addr, sizeof(out_addr));
  out_addr.sin_family = AF_INET;
  out_addr.sin_port = htons(7474);
  int err = inet_pton(AF_INET, target.c_str(), &out_addr.sin_addr);
  if(err != 1)
  {
    throw runtime_error("invalid sensor target address");
  }
  Sim::get().sensorManager.add(*this);
}
