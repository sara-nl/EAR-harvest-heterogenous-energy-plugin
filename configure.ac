# INCLUDES
m4_include([m4/ax_compare_version.m4])
m4_include([m4/x_ac_backup.m4])
m4_include([m4/x_ac_cpupower.m4])
m4_include([m4/x_ac_freeipmi.m4])
m4_include([m4/x_ac_slurm.m4])
m4_include([m4/x_ac_papi.m4])
m4_include([m4/x_ac_gsl.m4])

# INIT
AC_PREREQ([2.69])
AC_INIT([EAR], [1.0])
AC_LANG(C)

# PROGRAMS TEST
AC_PROG_CC
AC_PROG_AWK
AC_PROG_GREP

##############
# PAPIS TEST #
##############
#
# Installation directories:
#   --prefix=PREFIX         install architecture-independent files in PREFIX [/usr/local]
#   --exec-prefix=EPREFIX   install architecture-dependent files in EPREFIX [PREFIX]
#
# By default, `make install' will install all the files in
# `/usr/local/bin', `/usr/local/lib' etc.  You can specify
# an installation prefix other than `/usr/local' using `--prefix',
# for instance `--prefix=$HOME'.
#
# For better control, use the options below.
# Fine tuning of the installation directories:
#   --bindir=DIR           user executables [EPREFIX/bin]
#   --sbindir=DIR          system admin executables [EPREFIX/sbin]
#   --libdir=DIR           object code libraries [EPREFIX/lib]
#   --includedir=DIR       C header files [PREFIX/include]
#

X_AC_PAPI

############
# GSL TEST #
############
#
# Installation directories:
#   --prefix=PREFIX         install architecture-independent files in PREFIX [/usr/local]
#   --exec-prefix=EPREFIX   install architecture-dependent files in EPREFIX [PREFIX]
#
# By default, `make install' will install all the files in
# `/usr/local/bin', `/usr/local/lib' etc.  You can specify
# an installation prefix other than `/usr/local' using `--prefix',
# for instance `--prefix=$HOME'.
#
# For better control, use the options below.
#
# Fine tuning of the installation directories:
#   --bindir=DIR            user executables [EPREFIX/bin]
#   --sbindir=DIR           system admin executables [EPREFIX/sbin]
#   --libdir=DIR            object code libraries [EPREFIX/lib]
#   --includedir=DIR        C header files [PREFIX/include]
#

X_AC_GSL

############
# CPUPOWER #
############
#

X_AC_CPUPOWER

#########
# SLURM #
#########
# Installation directories:
#     --prefix=PREFIX         install architecture-independent files in PREFIX [/usr/local]
#     --exec-prefix=EPREFIX   install architecture-dependent files in EPREFIX [PREFIX]
#
#   By default, `make install' will install all the files in
#   `/usr/local/bin', `/usr/local/lib' etc.  You can specify
#   an installation prefix other than `/usr/local' using `--prefix',
#   for instance `--prefix=$HOME'.
#
#   For better control, use the options below.
#
#   Fine tuning of the installation directories:
#     --bindir=DIR            user executables [EPREFIX/bin]
#     --sbindir=DIR           system admin executables [EPREFIX/sbin]
#     --libdir=DIR            object code libraries [EPREFIX/lib]
#     --includedir=DIR        C header files [PREFIX/include]
#

X_AC_SLURM

############
# FREEIPMI #
############
#

X_AC_FREEIPMI

####################
# EAR INSTALLATION #
####################
#

if test "x$prefix" = "xNONE";then
    prefix=/usr/local
fi
if test "x$exec_prefix" = "xNONE"; then
    exec_prefix=$prefix
fi
if test "x$libdir" = "x\${exec_prefix}/lib"; then
    libdir=$exec_prefix/lib
fi
if test "x$bindir" = "x\${exec_prefix}/bin"; then
    bindir=$exec_prefix/bin
fi
if test "x$sbindir" = "x\${exec_prefix}/sbin"; then
    sbindir=$exec_prefix/sbin
fi
if test "x$sysconfdir" = "x\${exec_prefix}/etc"; then
    sysconfdir=$exec_prefix/etc
fi

if test -z "$MPICC"; then
    MPICC=mpicc
fi

AC_SUBST(MPICC)

##########
# OUTPUT #
##########

COMPILABLE="yes"

AC_DEFUN([AC_DEPENDANCY_CHECK],
[
	AC_MSG_CHECKING($2)

	if test -n "$1"; then
		AC_MSG_RESULT(yes)
	else
		if test $3 == 1; then
			COMPILABLE=""
		fi
		AC_MSG_RESULT(no)
	fi
])

echo "----------------------------------------------- Summary --"
AC_DEPENDANCY_CHECK($PAPI_DIR, PAPI dependancy found, 1)
AC_DEPENDANCY_CHECK($GSL_DIR, GSL dependancy found, 1)
AC_DEPENDANCY_CHECK($CPUPOWER_DIR, CPUPower dependancy found, 1)
AC_DEPENDANCY_CHECK($SLURM_DIR, SLURM dependancy found, 0)
AC_DEPENDANCY_CHECK($FREEIPMI_DIR, FreeIPMI dependancy found, 1)
echo "----------------------------------------------------------"

if test -z $COMPILABLE; then
	AC_MSG_ERROR(not compilable)
fi

AC_CONFIG_FILES([Makefile
                 etc/module/ear-1.0
                 etc/slurm/ear.conf
                 etc/slurm/plugstack.conf
                 src/common/config.h])
AC_CONFIG_FILES([src/tests/check/check.sh], [chmod +x src/tests/check/check.sh])
AC_CONFIG_FILES([etc/scripts/environment/lib_vars.sh], [chmod +x etc/scripts/environment/lib_vars.sh])
AC_OUTPUT

