#!/bin/bash
cd src/kern/compile/PROJ2
bmake depend
bmake
if [ $? -eq 0 ]; then
    bmake install
    cd ../../../..
    ./boot.sh $@
fi
