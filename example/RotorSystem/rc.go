package main

import "github.com/cycps/cys"

const (
	basedir     = "/Users/ry/@/cycps/cys"
	rsc         = basedir + "/build/control/lib/RotorSpeedController"
	sim         = basedir + "/build/example/RotorSystem/sim/RotorSystem"
	simsettings = basedir + "/build/example/RotorSystem/sim/RotorSystem"
	xpdir       = basedir + "/example/RotorSystem/control"
)

func main() {
	xp.Name("rc")
	controllers := []string{"rca", "rcb", "rcc"}

	for _, c := range controllers {
		cfg := c + ".yaml"
		fm := xp.FileMap{xpdir + "/" + cfg, "/cyp/" + cfg}
		xp.NewController(c, rsc, fm.Remote).AddFileMap(fm)
	}

	xp.SetSim(sim, xpdir+"/sim.yaml")

	xp.Main()
}
