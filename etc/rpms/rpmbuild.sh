#!/bin/bash

export EAR_INSTALL_PATH="/home/xjcorbalan/earc"
export EAR_SOURCE_PATH="/home/xjcorbalan/git/ear_private"

#
ln -s specs/ear.spec ear.spec
#ln -s specs/ear.intel.openmpi.spec ear.spec
#ln -s specs/ear.openmpi.spec ear.spec

#
rpmbuild -bb ear.spec

#
rm ear.spec
