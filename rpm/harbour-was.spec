Name:       harbour-was

Summary:    The unofficial application for Synology Photos on SailfishOS
Version:    1.0
Release:    1
License:    LICENSE
URL:        https://github.com/Kormil/was
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   sailfishsecretsdaemon
Requires:   sailfishsecretsdaemon-secretsplugin-common
Requires:   sailfishsecretsdaemon-secretsplugins-default
Requires:   sailfishsecretsdaemon-cryptoplugins-default
Requires:   sailfishsecretsdaemon-cryptoplugins-gnupg
Requires:   sailfishsecrets-tool
Requires:   sqlcipher
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(sailfishsecrets)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  desktop-file-utils
BuildRequires:  cmake

%description
Short description of my Sailfish OS Application


%prep
%setup -q -n %{name}-%{version}

%build

%cmake

%make_build


%install
%make_install


desktop-file-install --delete-original         --dir %{buildroot}%{_datadir}/applications                %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
