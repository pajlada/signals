#!/bin/sh

set -e

echo "Build type: ${BUILD_TYPE}"

cat /etc/os-release

cmake -GNinja --log-level=DEBUG --log-context --debug-output --debug-find --preset debug -B build

cmake --build build

ctest --test-dir build
