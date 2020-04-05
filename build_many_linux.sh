#!/usr/bin/env bash
# Script to build wheels for manylinux. This script executes bimpy_docker.sh inside docker

docker run -it --rm -e PLAT=manylinux2014_x86_64 -v `pwd`:/io quay.io/pypa/manylinux2014_x86_64 sh /io/docker.sh
