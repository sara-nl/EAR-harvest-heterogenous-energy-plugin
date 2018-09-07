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
	AC_SUBST(CC_FLAGS)
	AC_SUBST(MPICC_FLAGS)
	AC_SUBST(OMPICC_FLAGS)
	AC_SUBST(BUILD_TYPE)
    AC_SUBST(DB_FILES)
    AC_SUBST(DB_MYSQL)
])
