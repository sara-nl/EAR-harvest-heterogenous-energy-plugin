##*****************************************************************************
## $Id$
##*****************************************************************************
#  AUTHOR:
#    Jordi Gómez
#
#  SYNOPSIS:
#    X_AC_FREEIPMI()
#
#  DESCRIPTION:
#    Check the usual suspects for a freeipmi installation,
#    updating CPPFLAGS and LDFLAGS as necessary.
#
#  WARNINGS:
#    This macro must be placed after AC_PROG_CC and before AC_PROG_LIBTOOL.
##*****************************************************************************

AC_DEFUN([X_AC_FREEIPMI],
[
    _x_ac_freeipmi_dirs_root="/usr /usr/local /opt "
    _x_ac_freeipmi_dirs_libs="lib64 lib"
    _x_ac_freeipmi_gcc_libs="-lfreeipmi"
    _x_ac_freeipmi_gcc_ldflags=
    _x_ac_freeipmi_dir_bin=
    _x_ac_freeipmi_dir_lib=

    AC_ARG_WITH(
        [freeipmi],
        AS_HELP_STRING(--with-freeipmi=PATH,Specify path to FreeIPMI installation),
        [_x_ac_freeipmi_dirs_root="$withval", _x_ac_custom="yes"]
    )

    AC_CACHE_CHECK(
        [for FreeIPMI root directory],
        [_cv_freeipmi_dir_root],
        [
            for d in $_x_ac_freeipmi_dirs_root; do
                test -d "$d" || continue
                test -d "$d/include" || continue
                test -d "$d/include/freeipmi" || continue
                test -f "$d/include/freeipmi/freeipmi.h" || continue
                    for dir_lib in $_x_ac_freeipmi_dirs_libs;
                    do
                        # Testing if the library folder exists
                        test -d $d/$dir_lib || continue

                        # If exists, then its path and LDFLAGS are saved
                        if test -d "$d/$dir_lib"; then
                            _x_ac_freeipmi_dir_lib="$d/$dir_lib"

 							if test "x$_x_ac_custom" = "xyes"; then
                            	_x_ac_freeipmi_gcc_ldflags=-L$_x_ac_freeipmi_dir_lib
							fi
                        fi

                        X_AC_VAR_BACKUP([],[$_x_ac_freeipmi_gcc_ldflags],[$_x_ac_freeipmi_gcc_libs])
                        #
                        # AC_LANG_CALL(prologue, function) creates a source file
                        # and calls to the function.
                        # AC_LINK_IFELSE(input, action-if-true) runs the compiler
                        # and linker using LDFLAGS and LIBS.
                        #
                        AC_LINK_IFELSE(
                            [AC_LANG_CALL([], ipmi_ctx_create)],
                            [ipmi_open_inband=yes]
                        )
                        X_AC_VAR_UNBACKUP

                        if test "x$ipmi_open_inband" = "xyes"; then
                            AS_VAR_SET(_cv_freeipmi_dir_root, $d)
                        fi

                        test -n "$_cv_freeipmi_dir_root" && break
                    done
                test -n "$_cv_freeipmi_dir_root" && break
            done
        ]
    )

    if test -z "$_cv_freeipmi_dir_root"; then
        echo checking for FreeIPMI compiler link... no
    else
        FREEIPMI_DIR=$_cv_freeipmi_dir_root
        FREEIPMI_LIBDIR=$_x_ac_freeipmi_dir_lib
        FREEIPMI_CPPFLAGS="-I$FREEIPMI_DIR/include"
        FREEIPMI_LDFLAGS=$_x_ac_freeipmi_gcc_ldflags
        FREEIPMI_LIBS=$_x_ac_freeipmi_gcc_libs
        echo checking for FreeIPMI compiler link... yes
        echo checking for FreeIPMI CPPFLAGS... $FREEIPMI_CPPFLAGS
        echo checking for FreeIPMI LDFLAGS... $FREEIPMI_LDFLAGS
        echo checking for FreeIPMI libraries... $FREEIPMI_LIBS
    fi

    AC_SUBST(FREEIPMI_LIBS)
    AC_SUBST(FREEIPMI_LIBDIR)
    AC_SUBST(FREEIPMI_CPPFLAGS)
    AC_SUBST(FREEIPMI_LDFLAGS)
    AC_SUBST(FREEIPMI_DIR)

    AM_CONDITIONAL(WITH_FREEIPMI, test -n "$_cv_freeipmi_dir_root")
])
