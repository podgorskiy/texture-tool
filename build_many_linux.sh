#!/usr/bin/env bash
# Script to build wheels for manylinux. This script executes docker.sh inside docker
# sudo rm -R build
sudo rm -R wheelhouse

docker run -it --rm -e PLAT=manylinux2014_x86_64 -v `pwd`:/io quay.io/pypa/manylinux2014_x86_64 sh /io/docker.sh

me="$(whoami)"
sudo chown "$me" -R wheelhouse

# CXXABI_1.3.8 CXXABI_1.3.7


# CXXABI_1.3.5 CXXABI_1.3.3
# GLIBCXX_3.4.18  GLIBCXX_3.4.13
# GLIBC_2.15 GLIBC_2.12
