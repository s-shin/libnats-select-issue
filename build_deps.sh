#!/bin/bash
set -eu

repo_dir="$(cd "$(dirname "$0")"; pwd)"

: ${TMP_DIR:=${repo_dir}/tmp-deps}
: ${INSTALL_DIR:=$TMP_DIR}
: ${BUILD_TYPE:=Debug}

rm -rf "$TMP_DIR"
mkdir -p "$TMP_DIR"
cd "$TMP_DIR"

# hotfix "implicit declaration of function 'offsetof' is invalid in C99"
nats_c_path="${repo_dir}/deps/nats.c/src/nats.c"
if ! grep stddef.h "$nats_c_path" >/dev/null; then
  perl -i -pe 's/#include <stdio/#include <stddef.h>\n#include <stdio/' "$nats_c_path"
fi

cmake "${repo_dir}/deps/nats.c" \
  -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
  -DNATS_BUILD_TYPE="${BUILD_TYPE}" \
  -DNATS_BUILD_WITH_TLS=OFF \
  -DNATS_BUILD_EXAMPLES=OFF \
  -DNATS_BUILD_STREAMING=OFF

cmake --build .
cmake --build . -- install
