AC_DEFUN([X_AC_FIND_LIBRARY], [
	LIST_LD=`echo $LD_LIBRARY_PATH | tr ':' ' '`
	_x_ac_ld_dirs_root=
	for p in ${LIST_LD}; do
		_x_ac_ld_dirs_root="${_x_ac_ld_dirs_root} `echo $(cd ${p}/..; pwd)`"
	done
])
