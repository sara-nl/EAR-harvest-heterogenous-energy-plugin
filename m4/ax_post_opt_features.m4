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
	if test "x$DB_NAME" = "xpgsql"; then
		DB_MYSQL=0
		DB_PGSQL=1
	else
		DB_MYSQL=1
		DB_PGSQL=0
	fi

	#
	#
	#

	AC_SUBST(CC_FLAGS)
	AC_SUBST(MPI_VERSION)
	AC_SUBST(FEAT_AVX512)
	AC_SUBST(DB_MYSQL)
	AC_SUBST(DB_PGSQL)
	AC_SUBST(EAR_TMP)
])
