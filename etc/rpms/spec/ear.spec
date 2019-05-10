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
cp		-r	${EAR_INSTALL_PATH}/* %{buildroot}/usr/
exit

#
%files
%attr(-, $USER, $USER) /usr/*

%clean
rm -rf %{_topdir}/BUILD
rm -rf %{_topdir}/BUILDROOT
rm -rf %{_topdir}/SOURCES
rm -rf %{_topdir}/SPECS 
rm -rf %{_topdir}/SRPMS
