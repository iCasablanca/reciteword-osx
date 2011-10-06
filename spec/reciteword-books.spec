%define ver      0.8.4
%define rel      1
%define prefix   /usr

Summary: reciteword's book files
Name: reciteword-books
Version: %ver
Release: %rel
License: GPLv3
Group: Applications/Productivity
Source: reciteword-books-%{ver}.tar.bz2
BuildRoot: /var/tmp/%{name}-%{version}-root
BuildArchitectures: noarch

URL: http://reciteword.cosoft.org.cn

Requires: reciteword >= 0.8.4

%description
The book files of ReciteWord.

%prep
%setup -q -n books

%build
%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{prefix}/share/reciteword/books
cp -rf * $RPM_BUILD_ROOT%{prefix}/share/reciteword/books/

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)

%{prefix}/share/reciteword/books
