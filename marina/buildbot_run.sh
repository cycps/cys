#/usr/bin/env bash

docker run -itd --hostname=buildbot --name=buildbot -v `cd ..; pwd`:/cys cycps/builder
