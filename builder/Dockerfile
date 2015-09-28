FROM cycps/cys
RUN apt-get update && apt-get install -y \
  clang \
  build-essential \
  libc++1 libc++-dev \
  libc++abi1 libc++abi-dev \
  cmake \
  ninja-build \
  make \
  binutils

RUN mkdir /3p && \
  cd 3p && \
  curl -LO https://raw.githubusercontent.com/cycps/cys/master/builder/dependencies.make && \
  make -f dependencies.make

CMD service ssh start && /bin/bash
