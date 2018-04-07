#! /bin/bash
cd src/kern/compile/PROJ1
bmake depend
bmake
#bmake WERROR=
if [ $? -eq 0 ]; then
    bmake install
    cd ../../../..
    ./boot.sh $@
fi
