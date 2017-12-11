#!/bin/bash

function safexport
{
	if [ -z ${!1} ]
	then
		export $1=$2
	fi
}

safexport EAR 1
safexport EAR_DB_PATHNAME $EAR_INSTALL_PATH/DBS/system_db
safexport EAR_COEFF_DB_PATHNAME /home/xjcorbalan/EAR/COEFFICIENTS_TURBO/COEFF_COLLAB_
safexport EAR_USER_DB_PATHNAME $HOME/my_user_db
safexport EAR_DYNAIS_LEVELS 4
safexport EAR_DYNAIS_WINDOW_SIZE 200
safexport EAR_LEARNING_PHASE 0
safexport EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN 0.75
safexport EAR_PERFORMANCE_PENALTY 0.2
safexport EAR_PERFORMANCE_ACCURACY 100000
safexport EAR_RESET_FREQ 0
safexport EAR_TURBO 0
safexport EAR_TMP $EAR_INSTALL_PATH/tmp
safexport EAR_GUI_PATH $EAR_TMP
safexport EAR_VERBOSE 4
