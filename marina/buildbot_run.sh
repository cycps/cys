#/usr/bin/env bash

docker run -itd --hostname=buildbot --name=buildbot -v ~/@/cycps/cys:/cys cycps/builder
