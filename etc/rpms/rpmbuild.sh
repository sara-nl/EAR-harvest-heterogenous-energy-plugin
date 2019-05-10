#!/bin/bash

export EAR_INSTALL_PATH="/home/xjaneas/my_ear"

#
ln -s SPECS/ear.spec .

#
rpmbuild -bb ear.spec

#
rm ear.spec
