#!/usr/bin/env bash

docker network create -d overlay rcnet
docker service publish rca.rcnet
docker service publish rcb.rcnet
docker service publish rcc.rcnet
docker service publish sim.rcnet

docker run -itd --hostname=rca --name=rca -v `cd ../../; pwd`:/cys cycps/cys
docker run -itd --hostname=rcb --name=rcb -v `cd ../../; pwd`:/cys cycps/cys
docker run -itd --hostname=rcc --name=rcc -v `cd ../../; pwd`:/cys cycps/cys
docker run -itd --hostname=sim --name=sim -v `cd ../../; pwd`:/cys cycps/cys

docker service attach rca rca.rcnet
docker service attach rcb rcb.rcnet
docker service attach rcc rcc.rcnet
docker service attach sim sim.rcnet
