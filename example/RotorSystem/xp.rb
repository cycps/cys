#!/usr/bin/env ruby

controller = "/cys/build/control/lib/RotorSpeedController"
sim = "/cys/build/example/RotorSystem/sim/RotorSystem"
xpdir = "/cys/example/RotorSystem/control"

["rca", "rcb", "rcc"].each { |c| xp.nodes.push(controller, ["#{xpdir}/#{c}.yaml"]) }
xp.nodes.push(sim, [])

