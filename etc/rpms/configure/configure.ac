AC_PREREQ([2.69])
AC_INIT([EAR], [3.0])

if test "x$sysconfdir" = "x\${exec_prefix}/etc" || test "x$sysconfdir" = "x\${prefix}/etc"; then
	sysconfdir=$prefix/etc
fi
if test "x$localstatedir" = "x\${prefix}/var"; then
	localstatedir=/var/ear
fi


AC_CONFIG_FILES([
	etc/conf/ear.conf
	etc/module/ear
	etc/systemd/eard.service
	etc/systemd/eardbd.service
	etc/systemd/eargmd.service
	etc/slurm/ear.plugstack.conf])
AC_OUTPUT
