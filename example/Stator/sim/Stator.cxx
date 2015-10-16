#include "Stator.hxx"

using namespace cys;
using namespace cys::power;

int main()
{
  Stator s{"s", 5, 200, 5000, 60};

  return Sim::get().run(0, 5, 1e-3);
}
