%define ver      0.8.2
%define rel      1
%define prefix   /usr

Summary: reciteword's rw_en skin.
Name: reciteword-skins-rw_en
Version: %ver
Release: %rel
Copyright: GPL
Group: Applications/Productivity
Source: reciteword-skins-rw_en-%{ver}.tar.bz2
BuildRoot: /var/tmp/%{name}-%{version}-root
BuildArchitectures: noarch


URL: http://reciteword.cosoft.org.cn

Requires: reciteword >= 0.8.2

%description
ReciteWord's rw_en skin files.

%prep
%setup

%build
%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{prefix}/share/reciteword/skins/rw_en
cp -rf * $RPM_BUILD_ROOT%{prefix}/share/reciteword/skins/rw_en

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)

%{prefix}/share/reciteword/skins/rw_en
