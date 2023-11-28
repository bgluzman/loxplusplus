#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# TODO (bgluzman): get rid of horrendous hard-coding
COMPILER="$DIR/cmake-build-debug-wsl/lox++"

WORK_DIR="$(mktemp -d)"
function cleanup {
  popd >/dev/null
  rm -r "$WORK_DIR"
}
pushd "$WORK_DIR" >/dev/null
trap cleanup EXIT

eval "$COMPILER" <<<"$(cat -)" >"$WORK_DIR/program.ll"
eval "$DIR/misc/executable/create-executable.sh" "$WORK_DIR/program" <"$WORK_DIR/program.ll"
eval "$WORK_DIR/program"