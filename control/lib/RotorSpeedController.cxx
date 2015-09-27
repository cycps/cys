#include <fstream>
#include "Controller.hxx"
#include <string>
#include <signal.h>
#include <yaml-cpp/yaml.h>

using std::ofstream;
using namespace cys;
using std::endl;
using std::to_string;
using std::string;

struct RotorSpeedController : public Controller
{
  size_t rw_idx;
  size_t wt{6};
  string config_file;
  unsigned long w_tag, tau_tag;
  string tau_addr;

  RotorSpeedController(string config_file) 
    : Controller("rsc"), config_file{config_file}
  {
    readConfig();
    std::cout << "w.tag:  " << w_tag << std::endl;
    std::cout << "tau.tag:  " << tau_tag << std::endl;
    std::cout << "tau.address:  " << tau_addr << std::endl;

    rw_idx = setInput(w_tag);
    setDestination(tau_addr);
    setTarget(tau_tag);
  }

  void readConfig()
  {
    YAML::Node config = YAML::LoadFile(config_file);
    w_tag = config["inputs"]["w"]["tag"].as<unsigned long>();
    tau_tag = config["outputs"]["tau"]["tag"].as<unsigned long>();
    tau_addr = config["outputs"]["tau"]["address"].as<string>();
  }
    
  void compute() override
  {
    double v = input_frame[rw_idx];
    k_lg << ts() << "rx: v = " << v << endl;
    double u = 5*(wt - v);
    tx(u);
    k_lg << ts() << "tx: u = " << u << endl;
  }
};

RotorSpeedController *rsc = nullptr;

void sigh(int sig)
{
  rsc->k_lg << log("killed by signal: SIGINT(" + to_string(sig) + ")") << endl;
  rsc->io_lg << log("killed by signal: SIGINT(" + to_string(sig) + ")") << endl;
  close(rsc->sockfd);
  exit(1);
}
  

int main(int argc, char **argv)
{
  signal(SIGINT, sigh);

  if(argc != 2) {
    std::cerr << "usage: RotorSpeeController <config_file>" << std::endl;
    return 1;
  }

  rsc = new RotorSpeedController(argv[1]);
  rsc->run();
}
