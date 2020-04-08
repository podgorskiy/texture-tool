#!/usr/bin/env bash
# Script to build wheels for manylinux. This script executes docker.sh inside docker
# sudo rm -R build
sudo rm -R wheelhouse

docker run -it --rm -e PLAT=manylinux2014_x86_64 -v `pwd`:/io quay.io/pypa/manylinux2014_x86_64 sh /io/docker.sh

me="$(whoami)"
sudo chown "$me" -R wheelhouse

./postfix.sh


# CXXABI_1.3.8 CXXABI_1.3.7
