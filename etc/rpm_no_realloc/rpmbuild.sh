#!/bin/bash

export EAR_INSTALL_PATH="/home/xjcorbalan/ear_sc"
export EAR_SOURCE_PATH="/home/xjcorbalan/git/ear_private"

#
ln -s specs/ear.spec .

#
rpmbuild -bb ear.spec

#
rm ear.spec
