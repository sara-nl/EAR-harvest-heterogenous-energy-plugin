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

	AS_IF([test "x$enable_power_monitoring" = "xyes"], [AC_DEFINE(POWER_MONITORING)])

	#
	# POWER MONITORING
	#

	AC_ARG_ENABLE([shared-memory],
		AS_HELP_STRING([--enable-shared-memory], [Enables the shared memory feature]))
])
