#!/bin/sh
set -ex

# mnemo
mkdir -p build-aux
aclocal -I m4
autoreconf -fi
