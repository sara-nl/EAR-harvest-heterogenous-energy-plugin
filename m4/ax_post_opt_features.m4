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
	AC_SUBST(CC_FLAGS)
	AC_SUBST(MPICC_FLAGS)
	AC_SUBST(MPI_VERSION)
	AC_SUBST(FEAT_AVX512)
])
