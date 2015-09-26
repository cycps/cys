FROM ubuntu:15.04
RUN apt-get update && apt-get install -y \
  libc++1 \
  libc++abi1 \
  vim \
  tmux \
  ssh \
  rsync \
  curl \
  iproute2

CMD service ssh start && /bin/bash
