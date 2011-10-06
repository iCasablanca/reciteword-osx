%define ver      0.8.2
%define rel      1
%define prefix   /usr

Summary: reciteword's rw skin.
Name: reciteword-skins-rw
Version: %ver
Release: %rel
Copyright: GPL
Group: Applications/Productivity
Source: reciteword-skins-rw-%{ver}.tar.bz2
BuildRoot: /var/tmp/%{name}-%{version}-root
BuildArchitectures: noarch

URL: http://reciteword.cosoft.org.cn

Requires: reciteword >= 0.8.2

%description
ReciteWord's rw skin files.

%prep
%setup

%build
%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{prefix}/share/reciteword/skins/rw
cp -rf * $RPM_BUILD_ROOT%{prefix}/share/reciteword/skins/rw

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)

%{prefix}/reciteword/share/skins/rw
