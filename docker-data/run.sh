#!/bin/bash
set -eu

: ${FD_LIMIT:=2048}

ulimit -n "$FD_LIMIT"

gdb --args /usr/local/bin/libnats-select-issue "$@"
