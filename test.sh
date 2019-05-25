#!/bin/bash

# expects a checkout of https://github.com/nst/JSONTestSuite in this directory

set -e

for i in JSONTestSuite/test_parsing/*.json; do
  printf "%s... " $i
  if ! ./build/src/test-reader $i; then
    echo FAIL
    exit 1
  fi
  echo OK
done
