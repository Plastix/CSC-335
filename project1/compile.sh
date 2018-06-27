#! /bin/bash
cd src/kern/compile/PROJ2
bmake depend
bmake
#bmake WERROR=
bmake install
