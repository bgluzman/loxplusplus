#!/bin/bash

set -ex

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
WORK_DIR="$(mktemp -d)"
if [[ ! "$WORK_DIR" || ! -d "$WORK_DIR" ]]; then
  echo "Could not create temp dir"
  exit 1
fi
function cleanup {
  popd
  rm -r "$WORK_DIR"
  echo "Deleted temp working directory $WORK_DIR"
}
pushd "$WORK_DIR"
trap cleanup EXIT

llc -filetype=obj -o "lox.o" <<<"$(cat -)"
gcc "$DIR/main.c" -c "main.o"
gcc "lox.o" "main.o" -o "$DIR/lox-program"