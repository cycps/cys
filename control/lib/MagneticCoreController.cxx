#include "Controller.hxx"

using std::to_string;

/*------------------------------------------------------------------------------
 * Magnetic Core Controller
 * ........................
 *
 * Observes and controls the flux through an electromagnets core by actuating
 * the current through its coil.
 *
 *               -------
 *        flux   | ctl |    i 
 *       *-----> |     | -------*
 *       |       -------        |
 *       |                      |
 *       |       +++++++        |
 *       |       |     |        |
 *       *-------| mag |<-------*
 *               +++++++
 */

struct MagneticCoreController : public SimpleController
{
  double *flux;

  MagneticCoreController(string config_file)
    : Controller("MagneticCoreController", config_file)
  {
    flux = input("flux");
  }

  void pre() override
  {
    klog("flux = " + to_string(*flux));
  }

  void compute() override
  {
    i = ?;
  }

  void post() override
  {
    klog("i = " + to_string(i));
  }
};
