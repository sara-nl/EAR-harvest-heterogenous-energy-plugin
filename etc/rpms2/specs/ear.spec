# Commands
# -i <pack.file>	installs package
# -e <pack.name>	uninstall package
# --prefix <path> 	sets the installation path
# --test			don't install, but tell if it would work or not
# -qpl				query + package + list, list all the files in RPM

# Var definitions
%define name		ear
%define release		1
%define version		1.0

# Information
# Prefix	Just for when missing any prefix
Summary:	EAR package
Group:		System
Packager:	EAR Team
URL:		https://github.com/BarcelonaSupercomputingCenter/ear_private
License:	GPL
Name:		%{name}
Version:	%{version}
Release:	%{release}
Source:		%{name}‑%{version}.tar.gz
Buildroot:	%{_topdir}/BUILDROOT
Prefix:		/usr

# Brief description of the project
%description
EAR RPM package includes the daemons, configuration files
and tools required to make EAR works in your cluster.

# RPMBuild context
%prep
mkdir	-p	%{buildroot}/usr
cp		-rp	${EAR_INSTALL_PATH}/* %{buildroot}/usr/
rm		-rf %{buildroot}/usr/etc
mkdir	-p  %{buildroot}/usr/etc/ear
mkdir	-p  %{buildroot}/usr/etc/ear/coeffs
mkdir	-p  %{buildroot}/usr/etc/module
mkdir	-p  %{buildroot}/usr/etc/systemd
mkdir	-p  %{buildroot}/usr/etc/slurm
cp		-p  ${EAR_SOURCE_PATH}/etc/module/ear.in %{buildroot}/usr/etc/module/
cp		-p  ${EAR_SOURCE_PATH}/etc/conf/ear.conf.in %{buildroot}/usr/etc/ear/
cp		-p  ${EAR_SOURCE_PATH}/etc/conf/ear.conf.full.in %{buildroot}/usr/etc/ear/
cp      -p  ${EAR_SOURCE_PATH}/etc/systemd/eard.service.in %{buildroot}/usr/etc/systemd/
cp      -p  ${EAR_SOURCE_PATH}/etc/systemd/eardbd.service.in %{buildroot}/usr/etc/systemd/
cp      -p  ${EAR_SOURCE_PATH}/etc/systemd/eargmd.service.in %{buildroot}/usr/etc/systemd/
cp      -p  ${EAR_SOURCE_PATH}/etc/slurm/ear.plugstack.conf.in %{buildroot}/usr/etc/slurm/
cp		-p  ${EAR_SOURCE_PATH}/etc/rpms/configure/configure %{buildroot}/usr/etc
exit

# RPMBuild context
%files
/usr/include/ear.h
/usr/bin/tools/coeffs_show
/usr/bin/tools/get_ear_events
/usr/bin/tools/learning_delete
/usr/bin/tools/get_active_nodes
/usr/bin/tools/coeffs_default
/usr/bin/tools/learning_show
/usr/bin/tools/learning_validate
/usr/bin/tools/coeffs_compute
/usr/bin/tools/coeffs_quality
/usr/bin/tools/coeffs_jobproj
/usr/bin/tools/coeffs_null
/usr/bin/tools/coeffs_compare
/usr/bin/econtrol
/usr/bin/eacct
/usr/bin/ereport
/usr/bin/kernels
/usr/bin/scripts
/usr/etc/slurm/ear.plugstack.conf.in
/usr/etc/systemd/eardbd.service.in
/usr/etc/systemd/eargmd.service.in
/usr/etc/systemd/eard.service.in
/usr/etc/module/ear.in
/usr/etc/ear/coeffs
/usr/etc/ear/ear.conf.in
/usr/etc/ear/ear.conf.full.in
/usr/lib/libear_api.a
/usr/lib/libear.so
/usr/lib/plugins/policies/monitoring.so
/usr/lib/plugins/policies/min_energy.so
/usr/lib/plugins/policies/dummy.so
/usr/lib/plugins/policies/min_time_cp.so
/usr/lib/plugins/policies/two_phases.so
/usr/lib/plugins/policies/empty.so
/usr/lib/plugins/policies/min_time_no_models.so
/usr/lib/plugins/policies/min_energy_no_models.so
/usr/lib/plugins/policies/min_time.so
/usr/lib/plugins/models/basic_model.so
/usr/lib/plugins/models/per_inst_model.so
/usr/lib/plugins/energy/ipmi_lenovo_sd650.so
/usr/lib/plugins/energy/ipmi_ibm.so
/usr/lib/plugins/energy/ipmi_node_manager.so
/usr/lib/earplug.so
/usr/etc/configure
/usr/sbin/edb_create
/usr/sbin/eard
/usr/sbin/eargmd
/usr/sbin/eardbd
/usr/sbin/edb_clean_pm

# RPMBuild context
%clean
rm -rf %{_topdir}/BUILD
rm -rf %{_topdir}/BUILDROOT
rm -rf %{_topdir}/SOURCES
rm -rf %{_topdir}/SPECS 
rm -rf %{_topdir}/SRPMS

# RPM context
%post
(cd ${RPM_INSTALL_PREFIX}/etc && ./configure --prefix=${RPM_INSTALL_PREFIX})
rm -f ${RPM_INSTALL_PREFIX}/etc/config.log
rm -f ${RPM_INSTALL_PREFIX}/etc/config.status
rm -f ${RPM_INSTALL_PREFIX}/etc/configure
