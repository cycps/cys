#include <netdb.h>
#include <stdexcept>
#include "Sensor.hxx"

using namespace cys;
using std::string;
using std::runtime_error;
using std::to_string;
  
Sensor::Sensor(string name, Var v, unsigned long rate, unsigned long id_tag, 
    string target) 
  : v{v}, rate{rate}, id_tag{id_tag}, nxt{static_cast<realtype>(rate)}, 
    target{target}, name{name}
{
  bzero(&out_addr, sizeof(out_addr));
  out_addr.sin_family = AF_INET;
  out_addr.sin_port = htons(7474);
  addrinfo *ai{nullptr};
  //int err = inet_pton(AF_INET, target.c_str(), &out_addr.sin_addr);
  int err = getaddrinfo(target.c_str(), nullptr, nullptr, &ai);
  if(err != 0)
  {
    const char *serr = gai_strerror(err);
    throw runtime_error(
        "invalid sensor target address " + 
        string("'")+target+string("': ") + 
        "("+to_string(err)+") " + string(serr)
    );
  }
  if(ai->ai_addr->sa_family == AF_INET)
  {
    sockaddr_in *sin = (sockaddr_in*)ai->ai_addr;
    char s[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &sin->sin_addr, s, INET_ADDRSTRLEN);
    Sim::get().lg << log(target + " --> " + string(s)) << std::endl;

    out_addr.sin_addr = sin->sin_addr;
  }
  Sim::get().sensorManager.add(*this);
}
