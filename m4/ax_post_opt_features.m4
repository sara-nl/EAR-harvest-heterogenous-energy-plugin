##*****************************************************************************
## $Id$
##*****************************************************************************
#  AUTHOR:
#    Jordi Gómez
#
#  DESCRIPTION:
##*****************************************************************************

AC_DEFUN([AX_POST_OPT_FEATURES],
[
	#
	#
	#

	AC_SUBST(TMP)
	AC_SUBST(MPICC)
	AC_SUBST(OMPICC)
	AC_SUBST(mpi_so)
	AC_SUBST(ompi_so)
	AC_SUBST(mpi_trace_so)
	AC_SUBST(ompi_trace_so)
	AC_SUBST(BUILD_TYPE)
	AC_SUBST(POWER_MONITORING)
    AC_SUBST(SHARED_MEMORY)
    AC_SUBST(DB_FILES)
    AC_SUBST(DB_MYSQL)
])
