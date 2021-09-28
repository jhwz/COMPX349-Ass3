#!/bin/bash

set -eux

python3 build.py

cp MICROBIT.hex /media/elliott/MICROBIT/

echo "Copied"
