#!/bin/bash

CPUPOWER_LIB_PATH="/usr/lib64"
FREEIPMI_LIB_PATH="/home/xjaneas/opt/lib"
PAPI_LIB_PATH="/home/xjaneas/opt/lib"

export LD_LIBRARY_PATH=$CPUPOWER_LIB_PATH:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$FREEIPMI_LIB_PATH:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$PAPI_LIB_PATH:$LD_LIBRARY_PATH

function check
{
    EXEC=`./$2 $4 2> /dev/null`
    RETVAL=$?

    if test $RETVAL -eq 0; then
            echo -e Checking for $3 ... "\e[92mok\e[0m"
    else
        if [ "$1" == "e" ]; then
            echo -e Checking for $3 ... "\e[31merror\e[0m"
        else
            echo -e Checking for $3 ... "\e[93mwarning\e[0m"
        fi
    fi
}

check e cpu_examinable "CPU examinable by the library"
check e cpu_aperf "CPU APERF counter available"
check e cpu_uncores "CPU uncores presence"
check w cpu_uncores_all "CPU uncores available (all)"
check w papi_version "PAPI version >="
check e papi_init "PAPI initialization"
check e papi_comp_available "PAPI perf counters component available" "perf_event"
check e papi_comp_enabled "PAPI perf counters component enabled" "perf_event"
check e papi_comp_available "PAPI perf uncore counters component available" "perf_event_uncore"
check w papi_comp_enabled "PAPI perf uncore counters component enabled" "perf_event_uncore"
check w papi_comp_available "PAPI libmsr component available" "libmsr"
check w papi_comp_enabled "PAPI libmsr component enabled" "libmsr"
check e papi_comp_available "PAPI rapl component available" "rapl"
check w papi_comp_enabled "PAPI rapl component enabled" "rapl"
check w gsl_version "GSL version >="
check w slurm_version "SLURM version >="
check w free_ipmi_version "Free IPMI version >="
check e module_ipmi_devintf "ipmi_devintf driver online"
check e module_acpi_cpufreq "acpi-cpufreq driver online"

rm core.*
