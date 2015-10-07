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
  std::atomic<realtype> p{0};
  unsigned long id_tag;

  Actuator(std::string name, Var v, Limits s_limit, Limits d_limit, 
      unsigned long id_tag);

  void actuate(realtype x);
  void resid() override;
  void clamp(realtype &x);

};

}

#endif
