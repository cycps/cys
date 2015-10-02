package main

import "github.com/cycps/cys"

const (
	basedir     = "/Users/ry/@/cycps/cys"
	mc          = basedir + "/build/control/lib/MCoreController"
	sim         = basedir + "/build/example/MagneticCore/sim/RotorSystem"
	simsettings = basedir + "/build/example/MagneticCore/sim/RotorSystem"
	xpdir       = basedir + "/example/MagneticCore"
)

var controllers = []string{"ctl"}

func main() {
	xp.Name("mc")
	xp.NewController("ctl", mc, xpdir+"/control/mc.yaml")
	xp.SetSim(sim, xpdir+"/sim/sim.yaml")
	xp.Main()
}
