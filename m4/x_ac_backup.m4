AC_DEFUN([X_AC_VAR_BACKUP], [
    CPPFLAGS_backup=$CPPFLAGS
    CPPFLAGS=$1
    LDFLAGS_backup=$LDFLAGS
    LDFLAGS=$2
    LIBS_backup=$LIBS
    LIBS=$3
])

AC_DEFUN([X_AC_VAR_UNBACKUP], [
    CPPFLAGS=$CPPFLAGS_backup
    LDFLAGS=$LDFLAGS_backup
    LIBS=$LIBS_backup
])


AC_DEFUN([X_AC_VAR_PRINT], [
    echo $CPPFLAGS
    echo $LDFLAGS
    echo $LIBS
])
