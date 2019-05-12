##*****************************************************************************
## $Id$
##*****************************************************************************
#  AUTHOR:
#    Jordi Gómez
#
#  DESCRIPTION:
##*****************************************************************************

AC_DEFUN([AX_PRE_OPT_FEATURES],
[
	#
	#
	#
	AC_ARG_VAR([TMP],[Defines the node local storage as 'var', 'tmp' or other tempfs file system (default: /var/ear) (you can alo use --localstatedir=DIR)])
	AC_ARG_VAR([ETC],[Defines the read-only single-machine data as 'etc' (default: EPREFIX/etc) (you can also use --sharedstatedir=DIR)])
	AC_ARG_VAR([MAN],[Defines the documentation directory (default: PREFIX/man) (you can use also --mandir=DIR)])
	AC_ARG_VAR([COEFFS],[Defines the coefficients store directory (default: EPREFIX/etc)])

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
	if test -n "$DOC"; then
		docdir=$DOC
	fi
	if test "x$docdir" = "x\${datarootdir}/doc/\${PACKAGE_TARNAME}"; then
        docdir=/share/doc/ear
    fi

	AC_SUBST(TMP)

	#
	# MPI
	#
	AC_ARG_VAR([MPICC],[Defines the Intel MPI compiler])
	AC_ARG_VAR([OMPICC],[Defines the Open MPI compiler])

	# !I && !O
	if test -z "$MPICC" && test -z "$OMPICC"; then
		MPICC=mpicc
	fi

	if echo "$CC" | grep -q "icc"; then
        CC_FLAGS=-static-intel
    fi

	if echo "$MPICC" | grep -q "mpiicc"; then	
		MPICC_FLAGS=-static-intel
	fi

	#
	# DATABASE
	#
    AC_ARG_ENABLE([database],
        AS_HELP_STRING([--enable-database=TYPE], [Stores the execution data in the selected database type: mysql (def) or files]))

	DB_MYSQL=0
	DB_FILES=0

	AS_IF([test -z "$enable_database" || test "x$enable_database" = "xmysql"],
	[
			DB_MYSQL=1
			DB_FILES=0
	])
	AS_IF([test "x$enable_database" = "xfiles"],
    [
            DB_MYSQL=0
            DB_FILES=1
    ])
	AS_IF([test "x$enable_database" = "xboth"],
    [
            DB_MYSQL=1
            DB_FILES=1
    ])

    #
    # INSTALLATION USER/GROUP
    #
	AC_ARG_VAR([USER],[Sets the owner user of your installed files])
	AC_ARG_VAR([GROUP],[Sets the owner group of your installed files])

])
