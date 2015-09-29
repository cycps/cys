package xp

import (
	"fmt"
	"os"
	"os/exec"
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
	Nodes []*Node
}

type FileMap struct {
	Local, Remote string
}

type Node struct {
	Name, Exe string
	Args      []string
	Files     map[string]string
}

var XP = new(Experiment)

func NewNode(name, exe string, args ...string) *Node {
	n := new(Node)
	n.Files = make(map[string]string)
	n.Name = name
	n.Exe = exe
	n.Args = args
	XP.Nodes = append(XP.Nodes, n)
	return n
}

func (n *Node) AddFileMap(fm FileMap) *Node {
	n.Files[fm.Local] = fm.Remote
	return n
}

func Name(name string) {
	XP.Name = name
}

func usage() {
	fmt.Fprintf(os.Stderr, "usage: xp [up|down|run]\n")
	os.Exit(1)
}

func cmdErr(err error, msg string, out []byte) {
	fmt.Fprintln(os.Stderr, msg)
	fmt.Fprintf(os.Stderr, "%s\n", err)
	fmt.Fprint(os.Stderr, string(out))
	os.Exit(1)
}

func up() {

	netname := XP.Name + "net"

	out, err := exec.Command(
		"docker", "network", "create", "-d", "overlay", netname).CombinedOutput()
	if err != nil {
		cmdErr(err, "Creating docker network "+netname+" failed", out)
	}

	for _, n := range XP.Nodes {
		out, err = exec.Command(
			"docker", "service", "publish", n.Name+"."+netname).CombinedOutput()
		if err != nil {
			cmdErr(err, "Publishing service for "+n.Name+"failed", out)
		}

		out, err = exec.Command(
			"docker", "run", "-itd", "--hostname="+n.Name, "--name="+n.Name,
			"-v", "/cys:/cys", "cycps/cys").Output()
		if err != nil {
			cmdErr(err, "Running container for "+n.Name+" failed", out)
		}

		out, err = exec.Command(
			"docker", "service", "attach", n.Name, n.Name+"."+netname).CombinedOutput()
		if err != nil {
			cmdErr(err, "Attaching "+n.Name+" container to the network failed", out)
		}

		out, err = exec.Command(
			"docker", "exec", n.Name, "mkdir", "-p", "/app").CombinedOutput()
		if err != nil {
			cmdErr(err, "Making /app dir failed on node "+n.Name, out)
		}
		out, err = exec.Command(
			"docker", "exec", n.Name, "mkdir", "-p", "/cyp").CombinedOutput()
		if err != nil {
			cmdErr(err, "Making /cyp dir failed on node "+n.Name, out)
		}
		out, err = exec.Command(
			"docker", "cp", n.Exe, n.Name+":/app/").CombinedOutput()
		if err != nil {
			cmdErr(err, "Copying executable to "+n.Name+" failed", out)
		}

		for local, remote := range n.Files {

			out, err = exec.Command(
				"docker", "cp", local, n.Name+":"+remote).CombinedOutput()
			if err != nil {
				cmdErr(err, fmt.Sprintf("Copying local file %s to remote destination %s failed",
					local, remote), out)
			}

		}
	}

}
