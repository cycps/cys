#ifndef CYS_ACTUATOR
#define CYS_ACTUATOR

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <thread>

#include "Cypress.hxx"

namespace cys {

struct Actuator : public Object
{
  Var v;
  realtype s_limit, d_limit, p{0}, dp{0};
  unsigned long id_tag;

  Actuator(std::string name, Var v, realtype s_limit, realtype d_limit, 
      unsigned long id_tag);

  void actuate(realtype x);
  void resid() override;
  void clamp(realtype &x);

};

}

#endif
