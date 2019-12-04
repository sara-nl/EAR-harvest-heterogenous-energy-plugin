#!/bin/bash

function fake_config
{
	export FAKE_PLUGSTACK="prefix=/hpc/opt/ear sysconfdir=/hpc/opt/ear/etc localstatedir=/var/run/ear default=on"
	export FAKE_PATH=$PWD
}

function fake_init
{	
	$FAKE_PATH/fakeslurm --action=init --context=full --plugstack $FAKE_PLUGSTACK
}

function fake_exit
{	
	$FAKE_PATH/fakeslurm --action=exit --context=full --plugstack $FAKE_PLUGSTACK
}

fake_config

echo $$
fake_init
echo $$

$1

fake_exit
