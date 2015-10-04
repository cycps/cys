/*------------------------------------------------------------------------------
 * Static Proportional Feedback Controller
 * .......................................
 *
 * Produces a control signal as a proportion of the difference between its
 * input signal and a static reference point. If you requre an actual reference
 * signal use the regular (non-static) Proportional Feedback Controller.
 *
 *               -------
 *          y    | ctl |    u 
 *       *-----> |     | -------*
 *       |       -------        |
 *       |                      |
 *       |       +++++++        |
 *       |       |     |        |
 *       *-------| sys |<-------*
 *               +++++++
 */

#include <signal.h>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>
#include "Controller.hxx"

using std::string;
using std::to_string;

namespace cys { namespace control {

struct SP : public Controller
{
  string config_file, out_addr;
  unsigned long in_tag, out_tag;
  size_t in_idx;
  double 
    Kp,  // proportional constant
    Rp;  // reference point

  SP(string name, string config_file)
    : Controller(name), config_file{config_file}
  {
    readConfig();
    in_idx = setInput(in_tag);
    setTarget(out_tag);
    setDestination(out_addr);
  }

  void readConfig()
  {
    YAML::Node config = YAML::LoadFile(config_file);
    in_tag = config["input"]["tag"].as<unsigned long>();
    out_tag = config["output"]["tag"].as<unsigned long>();
    out_addr = config["output"]["address"].as<string>();
    Kp = config["Kp"].as<double>();
    Rp = config["Rp"].as<double>();
    period = config["period"].as<size_t>();
  }

  void compute() override
  {
    double y = input_frame[in_idx];
    double u = Kp*(Rp - y);
    tx(u);
    k_lg << ts() << "y = " << y << std::endl;
    k_lg << ts() << "u = " << u << std::endl;
  }
};


}}

cys::control::SP *p = nullptr;

void sigh(int sig)
{
  p->k_lg << 
    cys::log("killed by signal: SIGINT(" + to_string(sig) + ")") << std::endl;
  p->io_lg << 
    cys::log("killed by signal: SIGINT(" + to_string(sig) + ")") << std::endl;
  close(p->sockfd);
  exit(1);
}
  

int main(int argc, char **argv)
{
  signal(SIGINT, sigh);

  if(argc != 3) {
    std::cerr << "usage: SP <name> <config_file>" << std::endl;
    return 1;
  }

  p = new cys::control::SP(argv[1], argv[2]);
  p->run();
  return 0;
}
