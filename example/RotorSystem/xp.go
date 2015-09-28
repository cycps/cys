package main

import "cyp/xp"

func main() {
	rsc := "/cys/build/control/lib/RotorSpeedController"
	sim := "/cys/build/example/RotorSystem/sim/RotorSystem"
	xpdir := "/cys/example/RotorSystem/control"
	rscs := []string{"rca", "rcb", "rcc"}

	for c := range rscs {
		xp.NewNode(c, rsc, xpdir+"/"+c+".yaml")
	}
	xp.NewNode("sim", sim)

	xp.Main()
}
