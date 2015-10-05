package main

import "github.com/cycps/cys"

const (
	basedir     = "/home/ry/@/cycps/cys"
	ctl         = basedir + "/build/control/lib/SP"
	sim         = basedir + "/build/example/SynchronousMachine/sim/TurbineHouse"
	xpdir       = basedir + "/example/SynchronousMachine"
	simsettings = xpidr + "/sim/sim.yaml"
)

var controllers = []string{"ctl"}

func main() {
	xp.Name("mc")
	xp.NewController("ctl", ctl, xpdir+"/control/ctl.yaml")
	xp.SetSim(sim, simsettings)
	xp.Main()
}
