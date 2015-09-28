#!/usr/bin/env bash

docker-machine rm marina

ssh -S mts0 -O exit rgoodfel@users.isi.deterlab.net
ssh -S mts1 -O exit rgoodfel@users.isi.deterlab.net


