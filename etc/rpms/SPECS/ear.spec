#
%define _topdir		$RPM_ROOT
%define name		ear
%define release		1
%define version		1.0
%define buildroot   %{_topdir}

#
BuildRoot:	%{buildroot}
Summary:	EAR package
Group:		System
Packager:	EAR Team
URL:		https://github.com/BarcelonaSupercomputingCenter/ear_private
License:	GPL
Name:		%{name}
Version:	%{version}
Release:	%{release}
Source:		%{name}‑%{version}.tar.gz

# Brief description of the project
%description
EAR package

%prep
cp -r $RPM_ROOT/content/bin $RPM_BUILD_ROOT/usr/bin

exit
