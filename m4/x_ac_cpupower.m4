##*****************************************************************************
## $Id$
##*****************************************************************************
#  AUTHOR:
#    Jordi Gómez
#
#  SYNOPSIS:
#    X_AC_CPUPOWER()
#
#  DESCRIPTION:
#    Check the usual suspects for a cpupower installation,
#    updating CPPFLAGS and LDFLAGS as necessary.
#
#  WARNINGS:
#    This macro must be placed after AC_PROG_CC and before AC_PROG_LIBTOOL.
##*****************************************************************************

AC_DEFUN([X_AC_CPUPOWER],
[
    _x_ac_cpupower_dirs_root="/usr /usr/local"
    _x_ac_cpupower_dirs_libs="lib64 lib"
    _x_ac_cpupower_gcc_libs="-lcpupower"
    _x_ac_cpupower_gcc_ldflags=
    _x_ac_cpupower_dir_bin=
    _x_ac_cpupower_dir_lib=

    AC_ARG_WITH(
        [cpupower],
        AS_HELP_STRING(--with-cpupower=PATH,Specify path to CPUPower installation),
        [_x_ac_cpupower_dirs_root="$withval", _x_ac_custom="yes"]
    )

    AC_CACHE_CHECK(
        [for CPUPower root directory],
        [_cv_cpupower_dir_root],
        [
            for d in $_x_ac_cpupower_dirs_root; do
                test -d "$d" || continue
                test -d "$d/include" || continue
                test -f "$d/include/cpufreq.h" || continue
                    for dir_lib in $_x_ac_cpupower_dirs_libs;
                    do
                        # Testing if the library folder exists
                        test -d $d/$dir_lib || continue

                        # If exists, then its path and LDFLAGS are saved
                        if test -d "$d/$dir_lib"; then
                            _x_ac_cpupower_dir_lib="$d/$dir_lib"

							if test "x$_x_ac_custom" = "xyes"; then
                            	_x_ac_cpupower_gcc_ldflags=-L$_x_ac_cpupower_dir_lib
							fi
                        fi

                        X_AC_VAR_BACKUP([],[$_x_ac_cpupower_gcc_ldflags],[$_x_ac_cpupower_gcc_libs])
                        #
                        # AC_LANG_CALL(prologue, function) creates a source file
                        # and calls to the function.
                        # AC_LINK_IFELSE(input, action-if-true) runs the compiler
                        # and linker using LDFLAGS and LIBS.
                        #
                        AC_LINK_IFELSE(
                            [AC_LANG_CALL([], cpufreq_cpu_exists)],
                            [cpupower_cpu_exists=yes]
                        )
                        X_AC_VAR_UNBACKUP

                        if test "x$cpupower_cpu_exists" = "xyes"; then
                            AS_VAR_SET(_cv_cpupower_dir_root, $d)
                        fi

                        test -n "$_cv_cpupower_dir_root" && break
                    done
                test -n "$_cv_cpupower_dir_root" && break
            done
        ]
    )

    if test -z "$_cv_cpupower_dir_root"; then
        echo checking for CPUPower compiler link... no
    else
        CPUPOWER_DIR=$_cv_cpupower_dir_root
        CPUPOWER_LIBDIR=$_x_ac_cpupower_dir_lib
        CPUPOWER_CPPFLAGS="-I$CPUPOWER_DIR/include"
        CPUPOWER_LDFLAGS=$_x_ac_cpupower_gcc_ldflags
        CPUPOWER_LIBS=$_x_ac_cpupower_gcc_libs

        echo checking for CPUPower compiler link... yes
        echo checking for CPUPower CPPFLAGS... $CPUPOWER_CPPFLAGS
        echo checking for CPUPower LDFLAGS... $CPUPOWER_LDFLAGS
        echo checking for CPUPower libraries... $CPUPOWER_LIBS
    fi

    AC_SUBST(CPUPOWER_LIBS)
    AC_SUBST(CPUPOWER_LIBDIR)
    AC_SUBST(CPUPOWER_CPPFLAGS)
    AC_SUBST(CPUPOWER_LDFLAGS)
    AC_SUBST(CPUPOWER_DIR)

    AM_CONDITIONAL(WITH_CPUPOWER, test -n "$_cv_cpupower_dir_root")
])
