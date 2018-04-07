#! /bin/bash
cd src/kern/compile/PROJ1
bmake depend
bmake
#bmake WERROR=
bmake install
