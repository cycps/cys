#!/usr/bin/env bash

#sync my code
rsync -a -e \
  "ssh -o StrictHostKeyChecking=no" --rsh "ssh rgoodfel@users.isi.deterlab.net ssh" \
  ~/@/cycps/cys \
  rgoodfel@node-0.marina.cypress:/
