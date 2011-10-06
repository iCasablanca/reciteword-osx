%define ver      0.8.2
%define rel      1
%define prefix   /usr

Summary: reciteword's dict data files
Name: reciteword-dicts
Version: %ver
Release: %rel
Copyright: GPL
Group: Applications/Productivity
Source: reciteword-dicts-%{ver}.tar.bz2
BuildRoot: /var/tmp/%{name}-%{version}-root
BuildArchitectures: noarch


URL: http://reciteword.cosoft.org.cn

Requires: reciteword >= 0.8.2

%description
ReciteWord's dict data files.

%prep
%setup

%build
%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{prefix}/share/reciteword/dicts
cp -f xdicten.idx xdict.lib $RPM_BUILD_ROOT%{prefix}/share/reciteword/dicts/

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)

%{prefix}/share/reciteword/dicts
