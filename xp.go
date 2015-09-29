package xp

import (
	"fmt"
	"os"
	"os/exec"
	"path"
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
		down()
	case "run":
		fmt.Println("running experiment")
		run()
	default:
		usage()
	}

}

type Experiment struct {
	Name        string
	Controllers []*Node
	Sim         *Node
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

func NewController(name, exe, config string) *Node {
	n := new(Node)
	n.Files = make(map[string]string)
	n.Name = name
	n.Exe = exe
	n.Args = []string{cfgPath(config)}
	n.Files[config] = cfgPath(config)
	XP.Controllers = append(XP.Controllers, n)
	return n
}

func SetSim(exe string, settings string) *Node {
	n := new(Node)
	n.Files = make(map[string]string)
	n.Name = "sim"
	n.Exe = exe
	n.Args = []string{cfgPath(settings)}
	XP.Sim = n
	n.Files[exe] = appPath(exe)
	n.Files[settings] = cfgPath(settings)
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

func cmdWarn(err error, msg string, out []byte) {
	fmt.Fprintln(os.Stderr, msg)
	fmt.Fprintf(os.Stderr, "%s\n", err)
	fmt.Fprint(os.Stderr, string(out))
}

func cmdErr(err error, msg string, out []byte) {
	cmdWarn(err, msg, out)
	os.Exit(1)
}

func initNode(n *Node) {
	netname := XP.Name + "net"

	out, err := exec.Command(
		"docker", "service", "publish", n.Name+"."+netname).CombinedOutput()
	if err != nil {
		cmdErr(err, "Publishing service for "+n.Name+"failed", out)
	}

	out, err = exec.Command(
		"docker", "run", "-itd", "--hostname="+n.Name, "--name="+n.Name,
		"-v", "/cys:/cys", "cycps/cys").CombinedOutput()
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
			cmdErr(err,
				fmt.Sprintf("Copying local file %s to remote destination %s failed",
					local, remote),
				out)
		}

	}

}

func up() {

	netname := XP.Name + "net"

	out, err := exec.Command(
		"docker", "network", "create", "-d", "overlay", netname).CombinedOutput()
	if err != nil {
		cmdErr(err, "Creating docker network "+netname+" failed", out)
	}

	for _, n := range XP.Controllers {
		initNode(n)
	}

	initNode(XP.Sim)

}

func shutdownNode(n *Node) {

	out, err := exec.Command("docker", "stop", "-t", "0", n.Name).CombinedOutput()
	if err != nil {
		cmdWarn(err, "Error shutting down node "+n.Name, out)
	}

	out, err = exec.Command("docker", "rm", n.Name).CombinedOutput()
	if err != nil {
		cmdWarn(err, "Error removing node "+n.Name, out)
	}

	sname := n.Name + "." + XP.Name + "net"
	out, err = exec.Command("docker", "service", "unpublish", sname).CombinedOutput()
	if err != nil {
		cmdWarn(err, "Error unpublishing service "+sname, out)
	}

}

func down() {

	for _, n := range XP.Controllers {
		shutdownNode(n)
	}

	shutdownNode(XP.Sim)

	netname := XP.Name + "net"
	out, err := exec.Command("docker", "network", "rm", netname).CombinedOutput()
	if err != nil {
		cmdWarn(err, "Error removing network "+netname, out)
	}

}

func runController(n *Node) {
	out, err :=
		exec.Command("docker", "exec", "-d", n.Name, appPath(n.Exe), n.Args[0]).
			CombinedOutput()
	if err != nil {
		cmdErr(err, "Error executing controller "+n.Name+
			" with executable "+n.Exe, out)
	}
}

func appPath(pth string) string {
	return "/app/" + path.Base(pth)
}

func cfgPath(pth string) string {
	return "/cyp/" + path.Base(pth)
}

func runSim() {
	n := XP.Sim
	out, err :=
		exec.Command("docker", "exec", "-d", n.Name, appPath(n.Exe)).
			CombinedOutput()
	if err != nil {
		cmdErr(err, "Error executing simulation", out)
	}
}

func run() {
	for _, n := range XP.Controllers {
		runController(n)
	}
	runSim()
	fmt.Println("simulation is running")
}
