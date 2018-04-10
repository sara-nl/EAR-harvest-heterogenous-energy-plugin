##*****************************************************************************
## $Id$
##*****************************************************************************
#  AUTHOR:
#    Jordi Gómez
#
#  DESCRIPTION:
##*****************************************************************************

AC_DEFUN([AX_OPT_FEATURES],
[
	#
	#
	#
	AC_ARG_VAR([MPICC],[Defines the MPI compiler (default: mpicc)])
	AC_ARG_VAR([TMP],[Defines the node local storage as 'var', 'tmp' or other tempfs file system (default: /var/ear)])

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
	if test -n "$TMP"; then
		localstatedir=$TMP
	fi
	if test "x$localstatedir" = "x\${prefix}/var"; then
        localstatedir=/var/ear
    fi

	AC_SUBST(TMP)
	AC_SUBST(MPICC)

	#
	#
	#

	AC_ARG_VAR([BUILD_TYPE],[Defines the type of compilation: release (def), release-lrz, development or debug])

    if test "x$BUILD_TYPE" = "xdevelopment"; then
    	BUILD_TYPE=0
	elif test "x$BUILD_TYPE" = "xdebug"; then
		BUILD_TYPE=1
	elif test "x$BUILD_TYPE" = "xrelease"; then
		BUILD_TYPE=2
	elif test "x$BUILD_TYPE" = "xrelease-lrz"; then
		BUILD_TYPE=3
	else
		BUILD_TYPE=2
	fi

	AC_SUBST(BUILD_TYPE)

    #
    # POWER MONITORING
    #

	AC_ARG_ENABLE([power-monitoring],
        AS_HELP_STRING([--enable-power-monitoring], [Enables the power monitoring feature]))

	AS_IF([test "x$enable_power_monitoring" = "xyes"],
		[POWER_MONITORING=1],[POWER_MONITORING=0])

	AC_SUBST(POWER_MONITORING)

	#
	# POWER MONITORING
	#

	AC_ARG_ENABLE([shared-memory],
		AS_HELP_STRING([--enable-shared-memory], [Enables the shared memory feature]))

    AS_IF([test "x$enable_shared_memory" = "xyes"],
        [SHARED_MEMORY=1],[SHARED_MEMORY=0])

    AC_SUBST(SHARED_MEMORY)
])
