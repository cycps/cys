#!/usr/bin/env bash

docker exec -d rca \
  /cys/build/control/lib/RotorSpeedController \
  /cys/example/RotorSystem/control/rca.yaml

docker exec -d rcb \
  /cys/build/control/lib/RotorSpeedController \
  /cys/example/RotorSystem/control/rcb.yaml

docker exec -d rcc \
  /cys/build/control/lib/RotorSpeedController \
  /cys/example/RotorSystem/control/rcc.yaml

docker exec -d sim /cys/build/example/RotorSystem/sim/RotorSystem
