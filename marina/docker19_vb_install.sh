#/usr/bin/env bash

docker-machine status marina &> /dev/null
if [ $? -ne 0 ]; then
  docker-machine create -d virtualbox \
    --virtualbox-boot2docker-url=http://sirile.github.io/files/boot2docker-1.9.iso \
    buildbot
fi
