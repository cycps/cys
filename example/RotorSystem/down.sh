#!/usr/bin/env bash

docker stop -t 0 rca; docker rm rca;
docker stop -t 0 rcb; docker rm rcb;
docker stop -t 0 rcc; docker rm rcc;
docker stop -t 0 sim; docker rm sim;

docker service unpublish rca.rcnet
docker service unpublish rcb.rcnet
docker service unpublish rcc.rcnet
docker service unpublish sim.rcnet
docker network rm rcnet

