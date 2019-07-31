# libnats-select-issue

```sh
export BUILD_TYPE=Debug
./build_deps.sh

mkdir build
cd build
cmake .. -GNinja -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
ninja

ulimit -n 2048
libnats-select-issue 1500
```

via docker:

```sh
docker build . -t libnats-select-issue

docker run --rm -it --cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
   -e 'FD_LIMIT=512' libnats-select-issue 1000 nats://host.docker.internal:4222
# => ERROR: fopen(/usr/local/bin/libnats-select-issue): Too many open files

docker run --rm -it --cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
  libnats-select-issue 1000 nats://host.docker.internal:4222
# => no problem

docker run --rm -it --cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
  -e 'FD_LIMIT=2048' libnats-select-issue 2000 nats://host.docker.internal:4222
# => crash!
```
