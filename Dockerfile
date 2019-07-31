FROM ubuntu:18.04

SHELL ["/bin/bash", "-lc"]
WORKDIR /workspace

RUN apt-get update && \
  apt-get install -y build-essential cmake gdb git && \
  apt-get clean && \
  rm -rf /var/lib/apt/lists/*

COPY . ./

ARG BUILD_TYPE=Debug

RUN BUILD_TYPE="$BUILD_TYPE" ./build_deps.sh && \
  mkdir build && \
  cd build && \
  cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE" && \
  cmake --build . && \
  mv libnats-select-issue /usr/local/bin/

ENTRYPOINT ["bash", "/workspace/docker-data/run.sh"]
