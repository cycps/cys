#ifndef CYS_SENSOR
#define CYS_SENSOR

#include "Cypress.hxx"

namespace cys {

struct Sensor
{
  Var v;
  unsigned long rate, id_tag;
  realtype nxt;
  sockaddr_in out_addr;
  std::string target;

  Sensor(Var v, unsigned long rate, unsigned long id_tag, std::string target);
};

}

#endif
