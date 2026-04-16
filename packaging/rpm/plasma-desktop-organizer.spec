Name:           plasma-desktop-organizer
Version:        0.2.0
Release:        1%{?dist}
Summary:        Fences-style desktop organizer for KDE Plasma

License:        MIT
URL:            https://github.com/quinnjr/plasma-desktop-organizer
Source0:        %{name}-%{version}.tar.gz
ExclusiveArch:  x86_64

BuildRequires:  cmake >= 3.22
BuildRequires:  extra-cmake-modules >= 6.0
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtdeclarative-devel
BuildRequires:  kf6-kcoreaddons-devel
BuildRequires:  kf6-kconfig-devel
BuildRequires:  kf6-kio-devel
BuildRequires:  kf6-kiconthemes-devel
BuildRequires:  kf6-ksvg-devel
BuildRequires:  kf6-kstatusnotifieritem-devel
BuildRequires:  kf6-kwindowsystem-devel
BuildRequires:  kf6-kcmutils-devel
BuildRequires:  kf6-kconfigwidgets-devel
BuildRequires:  kf6-kdbusaddons-devel
BuildRequires:  layer-shell-qt-devel >= 6.0

Requires:       qt6-qtbase
Requires:       qt6-qtdeclarative
Requires:       kf6-kcoreaddons
Requires:       kf6-kconfig
Requires:       kf6-kio
Requires:       kf6-kiconthemes
Requires:       kf6-ksvg
Requires:       kf6-kstatusnotifieritem
Requires:       kf6-kwindowsystem
Requires:       kf6-kcmutils
Requires:       kf6-kconfigwidgets
Requires:       kf6-kdbusaddons
Requires:       layer-shell-qt

%description
Plasma Desktop Organizer provides customizable fence containers that overlay
the native Plasma desktop, allowing organization of desktop items without
replacing the desktop shell.

Features include drag-and-drop file management, KIO integration, live updates
via KDirWatch, system tray control, and System Settings integration via KCM.

%prep
%autosetup

%build
%cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF
%cmake_build

%install
%cmake_install

%files
%license LICENSE
%doc README.md CHANGELOG.md
%{_bindir}/plasma-organizer
%{_qt6_plugindir}/plasma/kcms/systemsettings/kcm_plasma_organizer.so
%{_datadir}/applications/kcm_plasma_organizer.desktop
%{_datadir}/dbus-1/services/org.kde.plasma.desktoporganizer.service
%{_datadir}/kcm_plasma_organizer/
%{_datadir}/autostart/plasma-organizer.desktop

%changelog
* Wed Apr 16 2026 Joseph R. Quinn <423821+quinnjr@users.noreply.github.com> - 0.2.0-1
- Initial RPM package release
- System Settings (KCM) integration
- D-Bus interface for IPC and service control
- OrganizerSettings singleton for centralized configuration
- Process control from System Settings
- Live configuration reload via D-Bus
