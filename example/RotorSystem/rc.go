package main

import "github.com/cycps/cys"

func main() {
	xp.Name("rc")
	rsc := "/cys/build/control/lib/RotorSpeedController"
	sim := "/cys/build/example/RotorSystem/sim/RotorSystem"
	xpdir := "/cys/example/RotorSystem/control"
	rscs := []string{"rca", "rcb", "rcc"}

	for _, c := range rscs {
		xp.NewNode(c, rsc, xpdir+"/"+c+".yaml")
	}
	xp.NewNode("sim", sim)

	xp.Main()
}
