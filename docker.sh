#!/usr/bin/env bash
# Script to build wheels for manylinux. This script runs inside docker.
# See build_maylinux_wheels.sh

# git clone https://github.com/podgorskiy/pypvrtex.git
# cd pypvrtex
# git submodule update --init

mkdir 3dparty
mkdir sources
mkdir pypvrtex
cp -R /io/pypvrtex/* ./pypvrtex
cp -R /io/3dparty/* ./3dparty
cp -R /io/sources/* ./sources
cp /io/setup.py ./setup.py
cp /io/postfix_wheel.py ./postfix_wheel.py

for PYBIN in /opt/python/*/bin; do
    #"${PYBIN}/pip" install -r /io/dev-requirements.txt
    #"${PYBIN}/pip" wheel /io/ -w wheelhouse/
    "${PYBIN}/python" setup.py bdist_wheel -d /io/wheelhouse/
done

# Bundle external shared libraries into the wheels
for whl in /io/wheelhouse/*.whl; do
    auditwheel show "$whl"
    auditwheel repair "$whl" --plat $PLAT -w /io/wheelhouse/
done
