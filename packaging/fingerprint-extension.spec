Name:       fingerprint-extension
Summary:    JLR XW extension for fingerprint readers
Version:    0.0.1
Release:    1
Group:      Automotive/Libraries
License:    Apache-2.0
URL:        http://www.tizen.org2
Source0:    %{name}-%{version}.tar.bz2

BuildRequires:  python
BuildRequires:  desktop-file-utils
BuildRequires:  check-devel
BuildRequires:  libusb-devel
BuildRequires:  nss-devel
BuildRequires:  pixman-devel
BuildRequires:  vim

BuildRequires:  pkgconfig(eina)
BuildRequires:  pkgconfig(eet)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(ecore-evas)
BuildRequires:  pkgconfig(edje)
BuildRequires:  pkgconfig(efreet)
BuildRequires:  pkgconfig(eldbus)

%define debug_package %{nil}

%description
This is a Crosswalk extension that provides access to fingerprint-reading
hardware, written by Jaguar Land Rover for the Automotive Grade Linux
distribution.

%prep

%setup
%autosetup

%build
pushd libfprint-0.6.0
%configure
make %{?_smp_mflags}
%make_install
popd

pushd jansson-2.7
%configure
make %{?_smp_mflags}
%make_install
popd

export PKG_CONFIG_PATH=%{buildroot}%{_libdir}/pkgconfig
export CFLAGS=-I%{buildroot}%{_includedir}
export LDFLAGS=-L%{buildroot}%{_libdir}

pushd fingerprint-extension
%autogen
%configure
make %{?_smp_mflags}
popd

%install
pushd libfprint-0.6.0
%make_install
popd

pushd jansson-2.7
%make_install
popd

pushd fingerprint-extension
%make_install
popd

%check
pushd fingerprint-extension
make check
popd

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/tizen-extensions-crosswalk/libfingerprint.so*
%{_includedir}/libfprint/fprint.h
%{_libdir}/libfprint.so*
%{_libdir}/pkgconfig/libfprint.pc
%{_libdir}/udev/rules.d/60-fprint-autosuspend.rules
%{_includedir}/jansson*
%{_libdir}/libjansson.so*
%{_libdir}/pkgconfig/jansson.pc
%{_datadir}/pkgconfig/fingerprint.pc
