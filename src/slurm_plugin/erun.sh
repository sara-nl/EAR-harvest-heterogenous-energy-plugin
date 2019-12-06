#!/bin/bash

$PWD/erun --program=$1 --plugstack prefix=/hpc/opt/ear sysconfdir=/hpc/opt/ear/etc localstatedir=/var/run/ear default=off
