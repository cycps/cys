#!/usr/bin/env bash

#create the tunnels requred to get a docker-machine
./tunnel_up.sh

#create the docker machine, reminder a docker machine is a (client,host) pair
docker-machine create -d generic \
  --generic-ip-address localhost \
  --generic-ssh-port 54022 \
  --generic-ssh-user rgoodfel \
  --engine-install-url "https://experimental.docker.com" \
  --engine-opt kv-store=consul:localhost:8500 \
  --engine-opt label=com.docker.network.driver.overlay.bind_interface=eth5 \
  marina

#establish docker machine environment
eval "$(docker-machine env marina)"

#sync me code
rsync -a -e \
  "ssh -o StrictHostKeyChecking=no" --rsh "ssh rgoodfel@users.isi.deterlab.net ssh" \
  `cd ..; pwd` \
  rgoodfel@node-0.marina.cypress:/users/rgoodfel

#run the build machine with the synced code mounted as a volume
#docker run -d \
#  -v /users/rgoodfel/cys:/cys \
#  --hostname=build \
#  --name=build \
#  cycps/build

