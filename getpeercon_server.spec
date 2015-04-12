Summary: SELinux development and test utility
Name: getpeercon_server
Version: 20150411
Release: 0%{?dist}
License: GPLv2
Source: %{name}-%{version}.tar.gz
BuildRequires: libselinux-devel

%description
The getpeercon_server is a simple test utility used to test SELinux labeled
communication paths over IPv4, IPv6, and UNIX domain sockets.

%prep
%setup -n %{name}

%build
make build

%install
pwd
rm -rf "%{buildroot}"
mkdir -p "%{buildroot}/%{_bindir}"
make DESTDIR="%{buildroot}" install

%files
%doc LICENSE
%{_bindir}/getpeercon_server

%changelog
* Sat Apr 11 2015 Paul Moore <pmoore@redhat.com> - 20150412-0
- Updated directory structure and build
* Mon Dec 03 2012 Paul Moore <pmoore@redhat.com> - 20121203-0
- Initial release
