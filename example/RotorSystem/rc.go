package main

import "github.com/cycps/cys"

const (
	basedir     = "/Users/ry/@/cycps/cys"
	rsc         = basedir + "/build/control/lib/RotorSpeedController"
	sim         = basedir + "/build/example/RotorSystem/sim/RotorSystem"
	simsettings = basedir + "/build/example/RotorSystem/sim/RotorSystem"
	xpdir       = basedir + "/example/RotorSystem"
)

func main() {
	xp.Name("rc")
	controllers := []string{"rca", "rcb", "rcc"}

	for _, c := range controllers {
		xp.NewController(c, rsc, xpdir+"/control/"+c+".yaml")
	}

	xp.SetSim(sim, xpdir+"/sim/sim.yaml")

	xp.Main()
}
