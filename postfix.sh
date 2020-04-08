#!/usr/bin/env bash

for whl in wheelhouse/*.whl; do
    python postfix_wheel.py "$whl"
done
