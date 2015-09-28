#include "xp.hxx"

using std::string;
using std::vector;

struct RotorXP : public XP
{
  void build() override
  {
    string 
      controller = "/cys/build/control/lib/RotorSpeedController",
      sim = "/cys/build/example/RotorSystem/sim/RotorSystem",
      xpdir = "/cys/example/RotorSystem/control",

    vector<string> controllers = {"rca", "rcb", "rcc"};

    for(auto c : controllers) {
      nodes.push_back({c, {controller, xpidr+"/"+c+".yaml"}});
    }
    nodes.push_back({ {"sim", {sim}} })
  }
};

int main()
{
  RotorXP *xp = new RotorXP();
  return xp.main();
}
