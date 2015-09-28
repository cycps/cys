package xp

import (
	"fmt"
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

func Main() {

	fmt.Println("apple muffins")

}
