package main

import "github.com/cycps/cys"

const (
	basedir     = "/home/ry/@/cycps/cys"
	ctl         = basedir + "/build/control/lib/SP"
	sim         = basedir + "/build/example/MagneticCore/sim/MCoreSystem"
	simsettings = basedir + "/example/MagneticCore/sim/sim.yaml"
	xpdir       = basedir + "/example/MagneticCore"
)

var controllers = []string{"ctl"}

func main() {
	xp.Name("mc")
	xp.NewController("ctl", ctl, xpdir+"/control/ctl.yaml")
	xp.SetSim(sim, simsettings)
	xp.Main()
}
