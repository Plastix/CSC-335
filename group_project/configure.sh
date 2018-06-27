#! /bin/bash
cd src/
./configure --ostree=$PWD/../root 
bmake
bmake install 

cd kern/conf/
./config $1

cd ../compile/$1
bmake depend
bmake
bmake install