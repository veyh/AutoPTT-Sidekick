VERSION 0.6

builder:
  FROM +runner
  RUN \
    apt-get update && \
    apt-get -y install --no-install-recommends avr-libc gcc-avr && \
    rm -rf /var/apt/cache

runner:
  FROM debian:11
  RUN \
    apt-get update && \
    apt-get -y install --no-install-recommends \
      ca-certificates curl wget make gcc g++ libc6-dev git && \
    rm -rf /var/apt/cache && \
    \
    cd /opt && \
    curl -fsSL https://github.com/Kitware/CMake/releases/download/v3.23.1/cmake-3.23.1-linux-x86_64.tar.gz \
      | tar xz && \
    mv cmake-3.23.1-linux-x86_64 cmake && \
    cd cmake/bin && \
    ln -t /usr/bin -sr *

all:
  BUILD +build
  BUILD +test

build:
  FROM +builder
  COPY . /work/usb-cdc-keyboard
  WORKDIR /work/usb-cdc-keyboard

  RUN \
    mkdir -p /work/out && \
    for i in board-*.mk; do make -f $i; done && \
    mv *.hex /work/out

  SAVE ARTIFACT /work/out AS LOCAL build/earthly

test:
  FROM +runner

  COPY . /work/usb-cdc-keyboard
  WORKDIR /work/usb-cdc-keyboard

  RUN \
    cmake -B build && \
    cd build && \
    make && \
    ctest --output-on-failure
