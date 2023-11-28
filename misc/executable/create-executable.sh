#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

OUT_PATH="$1"
if [ -z "$OUT_PATH" ]; then
  OUT_PATH="$DIR/lox-program"
fi

WORK_DIR="$(mktemp -d)"
function cleanup {
  popd >/dev/null
  rm -r "$WORK_DIR"
}
pushd "$WORK_DIR" >/dev/null
trap cleanup EXIT

llc -filetype=obj -o "lox.o" <<<"$(cat -)"
gcc -c "$DIR/main.c"
gcc "lox.o" "main.o" -o "$OUT_PATH"