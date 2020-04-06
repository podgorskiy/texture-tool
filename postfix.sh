#!/usr/bin/env bash

for whl in wheelhouse/*-manylinux2014_x86_64.whl; do
    python postfix_wheel.py "$whl"
done
