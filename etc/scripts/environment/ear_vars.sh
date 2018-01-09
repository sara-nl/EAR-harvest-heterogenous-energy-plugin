#!/bin/bash

function safexport
{
	if [ -z ${!1} ]
	then
		export $1=$2
	fi
}

safexport EAR 1
safexport EAR_DB_PATHNAME /tmp/ear/system_db
safexport EAR_COEFF_DB_PATHNAME /home/xjaneas/ear/COEFFICIENTS/coeff.
#safexport EAR_USER_DB_PATHNAME 
safexport EAR_DYNAIS_LEVELS 4
safexport EAR_DYNAIS_WINDOW_SIZE 500
safexport EAR_LEARNING_PHASE 0
safexport EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN 0.75
safexport EAR_PERFORMANCE_PENALTY 0.1
safexport EAR_PERFORMANCE_ACCURACY 1000000
safexport EAR_RESET_FREQ 0
safexport EAR_TURBO 0
safexport EAR_TMP /tmp/earc
safexport EAR_GUI_PATH /tmp/ear
safexport EAR_VERBOSE 2
safexport EAR_P_STATE 1
safexport EAR_NUM_NODES 1
