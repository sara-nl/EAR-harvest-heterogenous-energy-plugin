#!/bin/bash

export EAR_INSTALL_PATH="/home/xjaneas/my_ear"
export EAR_SOURCE_PATH="/home/xjaneas/git/EAR"

#
ln -s specs/ear.spec .

#
rpmbuild -bb ear.spec

#
rm ear.spec
