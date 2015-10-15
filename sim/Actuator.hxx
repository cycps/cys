#ifndef CYS_ACTUATOR
#define CYS_ACTUATOR

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <thread>
#include <array>
#include <atomic>

#include "Cypress.hxx"

namespace cys {

using Limits = std::array<realtype, 2>;

struct Actuator : public Object
{
  Var v;
  Limits s_limit, //static limits of actuation
         d_limit; //dynamic limits of actuation
  unsigned long id_tag;
  std::atomic<realtype> p;

  Actuator(std::string name, Var v, Limits s_limit, Limits d_limit, 
      unsigned long id_tag, realtype initial=0);

  void actuate(realtype x);
  void resid() override;
  void clamp(realtype &x);

};

}

#endif
