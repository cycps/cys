package main

import "cyp/xp"

func main() {
	controller := "/cys/build/control/lib/RotorSpeedController"
	sim := "/cys/build/example/RotorSystem/sim/RotorSystem"
	xpdir := "/cys/example/RotorSystem/control"

	for c := range []string{"rca", "rcb", "rcc"} {
		xp.Nodes = append(xp.Nodes,
			Node{c, []string{controller, xpdir + "/" + c + ".yaml"}})
	}
	xp.Nodes = append(xp.Nodes, Node{sim, []string{}})

	xp.Main()
}
