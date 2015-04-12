Summary: Labeled networking test utility for SELinux
Name: getpeercon_server
Version: 20121204
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
make

%install
rm -rf "%{buildroot}"
mkdir -p "%{buildroot}/%{_bindir}"
install getpeercon_server "%{buildroot}/%{_bindir}"

%files
%doc LICENSE
%{_bindir}/getpeercon_server

%changelog
* Mon Dec  3 2012 Paul Moore <pmoore@redhat.com> - 20121203-0
- Initial release
