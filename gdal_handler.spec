Summary:         GDAL handler for the OPeNDAP Data server
Name:            gdal_handler
Version:         0.9.2
Release:         1
License:         LGPLv2+
Group:           System Environment/Daemons 
Source0:         http://www.opendap.org/pub/source/%{name}-%{version}.tar.gz
URL:             http://www.opendap.org/
Requires:        libdap >= 3.11.0
Requires:        bes >= 3.9.0
# Requires:        libgdal >= 1.8

BuildRoot:       %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:   libdap-devel >= 3.11.2
BuildRequires:   bes-devel >= 3.9.1
# BuildRequires:   libgdal-devel >= 1.8

%description
This is the GDAL handler for our data server. We hope it will serve any
file that can be read using the GDAL library.

%prep 
%setup -q

%build
%configure --disable-static --disable-dependency-tracking
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install INSTALL="install -p"

rm -f $RPM_BUILD_ROOT%{_libdir}/*.la
rm -f $RPM_BUILD_ROOT%{_libdir}/*.so
rm -f $RPM_BUILD_ROOT%{_libdir}/bes/*.la

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%dir %{_sysconfdir}/bes/
%dir %{_sysconfdir}/bes/handlers
%config(noreplace) %{_sysconfdir}/bes/handlers/gdal.conf
%{_libdir}/bes/libgdal_handler.so
%{_datadir}/hyrax/
%doc COPYING NEWS README

%changelog
