#! /bin/bash
cd src/kern/compile/PROJ3
bmake depend
bmake
#bmake WERROR=
bmake install
