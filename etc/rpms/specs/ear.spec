#
%define name		ear
%define release		1
%define version		1.0

#
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

# 
%prep
mkdir	-p	%{buildroot}/usr
cp		-rp	${EAR_INSTALL_PATH}/* %{buildroot}/usr/
rm		-rf %{buildroot}/usr/etc
mkdir	-p  %{buildroot}/usr/etc/conf
mkdir	-p  %{buildroot}/usr/etc/module
mkdir	-p  %{buildroot}/usr/etc/systemd
mkdir	-p  %{buildroot}/usr/etc/slurm
cp		-p  ${EAR_SOURCE_PATH}/etc/module/ear.in %{buildroot}/usr/etc/module/
cp		-p  ${EAR_SOURCE_PATH}/etc/conf/ear.conf.in %{buildroot}/usr/etc/conf/
cp      -p  ${EAR_SOURCE_PATH}/etc/systemd/eard.service.in %{buildroot}/usr/etc/systemd/
cp      -p  ${EAR_SOURCE_PATH}/etc/systemd/eardbd.service.in %{buildroot}/usr/etc/systemd/
cp      -p  ${EAR_SOURCE_PATH}/etc/systemd/eargmd.service.in %{buildroot}/usr/etc/systemd/
cp      -p  ${EAR_SOURCE_PATH}/etc/slurm/ear.plugstack.conf.in %{buildroot}/usr/etc/slurm/
cp		-p  ${EAR_SOURCE_PATH}/etc/rpms/configure/configure %{buildroot}/usr/
exit

#
%files
%attr(-, -, -) /usr/*

%clean
rm -rf %{_topdir}/BUILD
#rm -rf %{_topdir}/BUILDROOT
rm -rf %{_topdir}/SOURCES
rm -rf %{_topdir}/SPECS 
rm -rf %{_topdir}/SRPMS

%post
(cd ${RPM_INSTALL_PREFIX} && ./configure --prefix=${RPM_INSTALL_PREFIX})
rm		-f ${RPM_INSTALL_PREFIX}/config.log
rm		-f ${RPM_INSTALL_PREFIX}/config.status
rm		-f ${RPM_INSTALL_PREFIX}/configure
rm		-f ${RPM_INSTALL_PREFIX}/etc/module/ear.in
rm		-f ${RPM_INSTALL_PREFIX}/etc/conf/ear.conf.in
rm		-f ${RPM_INSTALL_PREFIX}/etc/systemd/eard.service.in
rm		-f ${RPM_INSTALL_PREFIX}/etc/systemd/eardbd.service.in
rm		-f ${RPM_INSTALL_PREFIX}/etc/systemd/eargmd.service.in
rm		-f ${RPM_INSTALL_PREFIX}/etc/slurm/ear.plugstack.conf.in
