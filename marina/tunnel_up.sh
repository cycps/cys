#!/usr/bin/env bash
ssh -M -S mts0 -fnNT -L 54022:node-0.marina.cypress:22 rgoodfel@users.isi.deterlab.net
ssh -M -S mts1 -fnNT -L 2376:node-0.marina.cypress:2376 rgoodfel@users.isi.deterlab.net
