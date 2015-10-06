package main

import (
	"github.com/cycps/cys"
	"os"
)

var (
	basedir     = os.Getenv("CYPATH")
	ctl         = basedir + "/build/control/lib/SP"
	sim         = basedir + "/build/example/SynchronousMachine/sim/TurbineHouse"
	xpdir       = basedir + "/example/SynchronousMachine"
	simsettings = xpdir + "/sim/sim.yaml"
)

var controllers = []string{"ctl"}

func main() {
	xp.Name("syncc")
	xp.NewController("f_ctl", ctl, xpdir+"/control/field_ctl.yaml")
	xp.NewController("w_ctl", ctl, xpdir+"/control/rotor_ctl.yaml")
	xp.SetSim(sim, simsettings)
	xp.Main()
}
