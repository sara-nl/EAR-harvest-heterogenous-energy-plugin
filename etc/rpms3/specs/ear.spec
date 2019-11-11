# Commands
# -i <pack.file>  installs package
# -e <pack.name>  uninstall package
# --prefix <path>   sets the installation path
# --test      don't install, but tell if it would work or not
# -qpl        query + package + list, list all the files in RPM

# Var definitions
%define __requires_exclude libmpi*.*$|libpapi.so.*

%define name    ear 
%define release   1   
%define version   3.0 

# Information
# Prefix  Just for when missing any prefix
Summary:  EAR package
Group:    System
Packager: EAR Team
URL:    https://github.com/BarcelonaSupercomputingCenter/ear_private
License:  GPL 2.1
Name:   %{name}
Version:  %{version}
Release:  %{release}
Source:   %{name}‑%{version}.tar.gz
Buildroot:  %{_topdir}/BUILDROOT
Prefix:   /usr

# Brief description of the project
%description
EAR RPM package includes the daemons, configuration files
and tools required to make EAR works in your cluster.

# RPMBuild context
%prep
mkdir -p  %{buildroot}/usr
mkdir -p  %{buildroot}/usr/include
mkdir -p  %{buildroot}/usr/lib
mkdir -p  %{buildroot}/usr/lib/plugins
mkdir -p  %{buildroot}/usr/sbin
mkdir -p  %{buildroot}/usr/bin
mkdir -p  %{buildroot}/usr/bin/tools
mkdir -p  %{buildroot}/etc/ear
mkdir -p  %{buildroot}/etc/ear/coeffs
mkdir -p  %{buildroot}/etc/systemd/system
mkdir -p  %{buildroot}/etc/module
mkdir -p  %{buildroot}/etc/slurm
cp    -rp ${EAR_INSTALL_PATH}/bin/tools/coeffs_show %{buildroot}/usr/bin/tools/
cp    -rp ${EAR_INSTALL_PATH}/bin/tools/coeffs_compute %{buildroot}/usr/bin/tools/
cp    -rp ${EAR_INSTALL_PATH}/bin/tools/coeffs_null %{buildroot}/usr/bin/tools/
cp    -rp ${EAR_INSTALL_PATH}/bin/eacct %{buildroot}/usr/bin/
cp    -rp ${EAR_INSTALL_PATH}/bin/ereport %{buildroot}/usr/bin/
cp    -rp ${EAR_INSTALL_PATH}/bin/econtrol %{buildroot}/usr/bin/
cp    -rp ${EAR_INSTALL_PATH}/include/ear.h %{buildroot}/usr/include/
cp    -p  ${EAR_SOURCE_PATH}/etc/module/ear %{buildroot}/etc/module/
cp    -p  ${EAR_SOURCE_PATH}/etc/conf/ear.conf %{buildroot}/etc/ear/
cp  -p  ${EAR_SOURCE_PATH}/etc/slurm/ear.plugstack.conf %{buildroot}/etc/slurm/
cp	-rp ${EAR_INSTALL_PATH}/lib/libear.so %{buildroot}/usr/lib/
cp	-rp ${EAR_INSTALL_PATH}/lib/libear.ompi.so %{buildroot}/usr/lib/
cp    -rp ${EAR_INSTALL_PATH}/lib/libear_api.a %{buildroot}/usr/lib/libear_api.a
cp    -rp ${EAR_INSTALL_PATH}/lib/plugins/* %{buildroot}/usr/lib/plugins/
cp	-rp ${EAR_INSTALL_PATH}/lib/earplug.so %{buildroot}/usr/lib/
cp  -p  ${EAR_SOURCE_PATH}/etc/systemd/eargmd.service %{buildroot}/etc/systemd/system
cp  -p  ${EAR_SOURCE_PATH}/etc/systemd/eardbd.service %{buildroot}/etc/systemd/system
cp  -p  ${EAR_SOURCE_PATH}/etc/systemd/eard.service %{buildroot}/etc/systemd/system
cp	-rp ${EAR_INSTALL_PATH}/sbin/eargmd %{buildroot}/usr/sbin/
cp	-rp ${EAR_INSTALL_PATH}/sbin/eardbd %{buildroot}/usr/sbin/
cp	-rp ${EAR_INSTALL_PATH}/sbin/eard %{buildroot}/usr/sbin/
exit

# RPMBuild context
%files
/usr/include/ear.h
/etc/slurm/ear.plugstack.conf
/etc/systemd/system/eardbd.service
/etc/systemd/system/eargmd.service
/etc/systemd/system/eard.service
/etc/module/ear
/etc/ear/coeffs
/etc/ear/ear.conf
%attr(-, -, -) /usr/lib/*
%attr(-, -, -) /usr/sbin/*
%attr(-, -, -) /usr/bin/*
%attr(-, -, -) /usr/bin/tools/*

# RPMBuild context
%clean
rm -rf %{_topdir}/BUILD
rm -rf %{_topdir}/BUILDROOT
rm -rf %{_topdir}/SOURCES
rm -rf %{_topdir}/SPECS
rm -rf %{_topdir}/SRPMS

# RPM context

