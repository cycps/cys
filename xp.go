package xp

import (
	"fmt"
	"os"
)

func Main() {

	if len(os.Args) < 2 {
		usage()
	}

	switch os.Args[1] {
	case "up":
		fmt.Println("bringing environment up")
		up()
	case "down":
		fmt.Println("bringing environment down")
	case "run":
		fmt.Println("running experiment")
	default:
		usage()
	}

}

type Experiment struct {
	Name  string
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

func Name(name string) {
	XP.Name = name
}

func usage() {
	fmt.Fprintf(os.Stderr, "usage: xp [up|down|run]\n")
	os.Exit(1)
}

func up() {
	out, err := exec.Cmd(
		"docker", "network", "create", "-d", "overlay", XP.Name+"net").Output()

	if err != nil {
		fmt.Fprintln(os.Stderr, "Creating docker network failed")
		fmt.Fprintf(os.Stderr, "%s\n", err)
		fmt.Fprintln(os.Stderr, string(out))
	}

	for _, n := range XP.Nodes {
	}
}
