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
	AC_ARG_VAR([TMP],[Defines the node local storage as 'var', 'tmp' or other tempfs file system (default: /var/ear)])
	AC_ARG_VAR([ETC],[Defines the read-only single-machine data as 'etc' (default: EPREFIX/etc)])

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
	if test -n "$ETC"; then
		sysconfdir=$ETC	
	fi
	if test "x$sysconfdir" = "x\${exec_prefix}/etc" || test "x$sysconfdir" = "x\${prefix}/etc"; then
    	sysconfdir=$prefix/etc
	fi
	if test -n "$TMP"; then
		localstatedir=$TMP
	fi
	if test "x$localstatedir" = "x\${prefix}/var"; then
        localstatedir=/var/ear
    fi

	AC_SUBST(TMP)

	#
	# MPI
	#
	AC_ARG_VAR([IMPICC],[Defines the Intel MPI compiler])
	AC_ARG_VAR([OMPICC],[Defines the Open MPI compiler])

	MPICC=
	mpi_so=
	mpi_trace_so=
	ompi_so=
	ompi_trace_so=

	# !I && !O
	if test -z "$IMPICC" && test -z "$OMPICC"; then
		IMPICC=mpicc
	fi

	# I 
	if test -n "$IMPICC"; then
		MPICC=$IMPICC
		mpi_so=libear.so
		mpi_trace_so=libeart.so	
	fi

	# O
	if test -n "$OMPICC"; then
        ompi_so=libear_ompi.so
        ompi_trace_so=libeart_ompi.so
    fi

	AC_SUBST(MPICC)
	AC_SUBST(OMPICC)
	AC_SUBST(mpi_so)
	AC_SUBST(ompi_so)
	AC_SUBST(mpi_trace_so)
	AC_SUBST(ompi_trace_so)

	#
	# BUILD TYPE
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
	# SHARED MEMORY
	#

	AC_ARG_ENABLE([shared-memory],
		AS_HELP_STRING([--enable-shared-memory], [Enables the shared memory feature]))

    AS_IF([test "x$enable_shared_memory" = "xyes"],
        [SHARED_MEMORY=1],[SHARED_MEMORY=0])

    AC_SUBST(SHARED_MEMORY)

	#
	# DATABASE
	#
    AC_ARG_ENABLE([database],
        AS_HELP_STRING([--disable-database], [Stores the execution data in files insted in a database]))


	AS_IF([test "x$enable_database" = "xno" || test -z "$enable_database"],
        [
			DATABASE=0
		],[
			DATABASE=1
			enable_database="yes"
		])

    AC_SUBST(DATABASE)
	
])
