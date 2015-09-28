package main

import "github.com/cycps/cys"

func main() {
	xp.Name("rc")
	rsc := "/Users/ry/@/cycps/cys/build/control/lib/RotorSpeedController"
	sim := "/Users/ry/@/cycps/cys/build/example/RotorSystem/sim/RotorSystem"
	xpdir := "/Users/ry/@/cycps/cys/example/RotorSystem/control"
	rscs := []string{"rca", "rcb", "rcc"}

	for _, c := range rscs {

		cfg := c + ".yaml"
		fm := xp.FileMap{xpdir + "/" + cfg, "/cyp/" + cfg}

		xp.NewNode(c, rsc, fm.Remote).
			AddFileMap(fm)

	}
	xp.NewNode("sim", sim)

	xp.Main()
}
