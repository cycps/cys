package xp

import (
	"fmt"
	"os"
)

type Experiment struct {
	Nodes []Node
}

type Node struct {
	Name, Cmd string
	Args      []string
}

var XP = new(Experiment)

func NewNode(name, cmd string, args ...string) {
	XP.Nodes = append(XP.Nodes, Node{name, cmd, args})
}

func usage() {
	fmt.Fprintf(os.Stderr, "usage: xp [up | down | run]\n")
	os.Exit(1)
}

func Main() {

	if len(os.Args) < 2 {
		usage()
	}

	switch os.Args[1] {
	case "up":
		fmt.Println("bringing environment up")
	case "down":
		fmt.Println("bringing environment down")
	case "run":
		fmt.Println("running experiment")
	default:
		usage()
	}

}
