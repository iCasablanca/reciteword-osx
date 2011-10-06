Name: reciteword
Summary: Recite Word Easily
Version: 0.8.4
Release: 1%{?dist}
Group: Applications/Productivity
License: GPLv3
URL: http://reciteword.sourceforge.net
Source: http://downloads.sourceforge.net/%{name}/%{name}-%{version}.tar.bz2
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

Requires: gtk2 >= 2.2
Requires: esound >= 0.2.28
BuildRequires: gettext gtk2-devel >= 2.2, esound-devel >= 0.2.28

%description
ReciteWord is an education software to help people to study English,
recite words.

%description -l zh_CN
黑客背单词。

%prep
%setup -q

%build
%configure
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install
%find_lang %{name}

%clean
rm -rf $RPM_BUILD_ROOT

%files -f %{name}.lang
%defattr(-, root, root, -)
%{_bindir}/reciteword
%{_datadir}/reciteword
%{_datadir}/applications/*
%{_datadir}/pixmaps/*
