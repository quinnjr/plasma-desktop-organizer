# Packaging Design: .deb and .rpm Distribution

**Date:** 2026-04-16  
**Status:** Approved  
**Version:** 0.2.x

## Overview

Implement automated package building for Debian (.deb) and Fedora (.rpm) distributions using GitHub Actions. Packages are built in CI/CD on every tagged release and automatically uploaded to GitHub releases.

## Goals

- Provide native .deb packages for Ubuntu 24.04 LTS, Ubuntu 24.10, and Debian 13
- Provide native .rpm packages for Fedora 40 and Fedora 41
- Fully automated builds triggered by git tags
- No local build environment required
- Packages install to system paths (`/usr`) for global availability
- Automatic dependency resolution

## Non-Goals

- Package signing (can be added later for official repositories)
- Repository hosting (packages downloaded from GitHub releases)
- ARM64 builds (can be added later if needed)
- Flatpak/AppImage/Snap (different distribution mechanisms)

## Architecture

### Build Pipeline

```
Developer                 GitHub Actions                  GitHub Releases
    |                           |                                |
    | git tag v0.2.1           |                                |
    | git push origin v0.2.1   |                                |
    |------------------------->|                                |
    |                          |                                |
    |                          | Trigger workflow               |
    |                          | Matrix: 5 distros              |
    |                          |                                |
    |                          | Build Ubuntu 24.04 .deb -------|
    |                          | Build Ubuntu 24.10 .deb -------|
    |                          | Build Debian 13 .deb ----------|
    |                          | Build Fedora 40 .rpm ----------|
    |                          | Build Fedora 41 .rpm ----------|
    |                          |                                |
    |                          | Upload artifacts               |
    |                          |------------------------------->|
    |                                                            |
    |<-----------------------------------------------------------|
    Users download packages from GitHub release
```

### Component Architecture

1. **Debian Packaging Files** (`packaging/debian/`)
   - Follow Debian Policy Manual
   - Use debhelper for automation
   - Standard three-clause format (control, rules, changelog)

2. **RPM Spec File** (`packaging/rpm/plasma-desktop-organizer.spec`)
   - Follow Fedora Packaging Guidelines
   - Use standard rpm macros
   - Separate %build, %install, %files sections

3. **GitHub Actions Workflow** (`.github/workflows/build-packages.yml`)
   - Two-job structure: build-deb, build-rpm
   - Matrix strategy for multiple distros
   - Artifact upload to release

4. **Build Scripts** (`packaging/scripts/`)
   - Wrapper scripts for consistency
   - Handle Docker container orchestration
   - Extract built packages

## Distribution Support Matrix

| Distribution | Version | Code Name | KF6 Version | Status |
|--------------|---------|-----------|-------------|--------|
| Ubuntu       | 24.04 LTS | Noble Numbat | 6.0+ | Primary |
| Ubuntu       | 24.10   | Oracular Oriole | 6.5+ | Current |
| Debian       | 13      | Trixie | 6.0+ | Testing |
| Fedora       | 40      | - | 6.0+ | Stable |
| Fedora       | 41      | - | 6.5+ | Current |

## Packaging Specifications

### Debian Package Structure

**Package name:** `plasma-desktop-organizer`  
**Version format:** `0.2.1-1` (upstream-debianrevision)  
**Architecture:** `amd64`  
**Section:** `kde`  
**Priority:** `optional`

**Files installed:**
```
/usr/bin/plasma-organizer
/usr/lib/x86_64-linux-gnu/qt6/plugins/plasma/kcms/systemsettings/kcm_plasma_organizer.so
/usr/share/applications/kcm_plasma_organizer.desktop
/usr/share/dbus-1/services/org.kde.plasma.desktoporganizer.service
/usr/share/kcm_plasma_organizer/*.qml
/etc/xdg/autostart/plasma-organizer.desktop
```

**Build dependencies:**
- cmake (>= 3.22)
- extra-cmake-modules (>= 6.0)
- qtbase6-dev, qtdeclarative6-dev, qtquickcontrols2-6-dev
- libkf6coreaddons-dev, libkf6config-dev, libkf6kio-dev
- libkf6iconthemes-dev, libkf6svg-dev
- libkf6statusnotifieritem-dev, libkf6windowsystem-dev
- libkf6kcmutils-dev, libkf6configwidgets-dev, libkf6dbusaddons-dev
- liblayershellqt-dev (>= 6.0)

**Runtime dependencies:** Automatically detected via `${shlibs:Depends}`

### RPM Package Structure

**Package name:** `plasma-desktop-organizer`  
**Version format:** `0.2.1-1.fc41` (version-release.distro)  
**Architecture:** `x86_64`  
**Group:** `Applications/Productivity`

**Files installed:**
```
/usr/bin/plasma-organizer
/usr/lib64/qt6/plugins/plasma/kcms/systemsettings/kcm_plasma_organizer.so
/usr/share/applications/kcm_plasma_organizer.desktop
/usr/share/dbus-1/services/org.kde.plasma.desktoporganizer.service
/usr/share/kcm_plasma_organizer/*.qml
/etc/xdg/autostart/plasma-organizer.desktop
```

**Build dependencies:**
- cmake >= 3.22
- extra-cmake-modules >= 6.0
- qt6-qtbase-devel, qt6-qtdeclarative-devel
- kf6-kcoreaddons-devel, kf6-kconfig-devel, kf6-kio-devel
- kf6-kiconthemes-devel, kf6-ksvg-devel
- kf6-kstatusnotifieritem-devel, kf6-kwindowsystem-devel
- kf6-kcmutils-devel, kf6-kconfigwidgets-devel, kf6-kdbusaddons-devel
- layer-shell-qt-devel >= 6.0

**Runtime dependencies:** Automatically detected via `Requires:` scanning

## GitHub Actions Workflow Design

### Workflow File: `.github/workflows/build-packages.yml`

**Triggers:**
- Push of tags matching `v*` pattern
- Manual workflow dispatch (for testing)

**Environment variables:**
- `VERSION`: Extracted from tag (e.g., v0.2.1 → 0.2.1)
- `DEBIAN_REVISION`: 1
- `RPM_RELEASE`: 1

### Job 1: build-deb

**Strategy matrix:**
```yaml
matrix:
  distro:
    - ubuntu:24.04
    - ubuntu:24.10
    - debian:13
  include:
    - distro: ubuntu:24.04
      distro_name: ubuntu
      distro_version: 24.04
    - distro: ubuntu:24.10
      distro_name: ubuntu
      distro_version: 24.10
    - distro: debian:13
      distro_name: debian
      distro_version: 13
```

**Steps:**
1. Checkout repository
2. Run Docker container with distro image
3. Install build dependencies:
   ```bash
   apt-get update
   apt-get install -y build-essential debhelper cmake extra-cmake-modules \
     qtbase6-dev qtdeclarative6-dev qtquickcontrols2-6-dev \
     libkf6coreaddons-dev libkf6config-dev libkf6kio-dev \
     libkf6iconthemes-dev libkf6svg-dev libkf6statusnotifieritem-dev \
     libkf6windowsystem-dev libkf6kcmutils-dev libkf6configwidgets-dev \
     libkf6dbusaddons-dev liblayershellqt-dev
   ```
4. Copy source to container at `/build/plasma-desktop-organizer-$VERSION`
5. Create orig tarball: `tar czf plasma-desktop-organizer_$VERSION.orig.tar.gz`
6. Run `dpkg-buildpackage -b -uc -us` (build binary, unsigned)
7. Extract `.deb` from `/build/`
8. Upload artifact: `plasma-desktop-organizer_$VERSION-1_amd64.deb`

**Runs in parallel:** 3 containers simultaneously

### Job 2: build-rpm

**Strategy matrix:**
```yaml
matrix:
  fedora_version: [40, 41]
```

**Steps:**
1. Checkout repository
2. Run Docker container with `fedora:$VERSION` image
3. Install build dependencies:
   ```bash
   dnf install -y cmake extra-cmake-modules gcc-c++ rpm-build rpmdevtools \
     qt6-qtbase-devel qt6-qtdeclarative-devel \
     kf6-kcoreaddons-devel kf6-kconfig-devel kf6-kio-devel \
     kf6-kiconthemes-devel kf6-ksvg-devel kf6-kstatusnotifieritem-devel \
     kf6-kwindowsystem-devel kf6-kcmutils-devel kf6-kconfigwidgets-devel \
     kf6-kdbusaddons-devel layer-shell-qt-devel
   ```
4. Create RPM build tree: `~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}`
5. Copy source tarball to `~/rpmbuild/SOURCES/`
6. Copy spec file to `~/rpmbuild/SPECS/`
7. Run `rpmbuild -bb ~/rpmbuild/SPECS/plasma-desktop-organizer.spec`
8. Extract `.rpm` from `~/rpmbuild/RPMS/x86_64/`
9. Upload artifact: `plasma-desktop-organizer-$VERSION-1.fc$FEDORA_VERSION.x86_64.rpm`

**Runs in parallel:** 2 containers simultaneously

### Job 3: create-release

**Depends on:** build-deb, build-rpm

**Steps:**
1. Download all artifacts (5 packages)
2. Create GitHub release from tag (if not exists)
3. Upload packages to release:
   ```bash
   gh release upload $TAG *.deb *.rpm
   ```

**Result:** Release page has 5 downloadable packages

## Build Scripts

### `packaging/scripts/build-deb.sh`

**Purpose:** Wrapper for Debian package building in Docker

**Arguments:**
- `$1`: Distro image (e.g., ubuntu:24.04)
- `$2`: Version (e.g., 0.2.1)
- `$3`: Output directory

**Process:**
1. Start container with distro image
2. Mount source directory read-only
3. Install build dependencies
4. Run dpkg-buildpackage
5. Copy .deb to output directory
6. Cleanup container

**Exit codes:**
- 0: Success
- 1: Build failed
- 2: Dependency installation failed

### `packaging/scripts/build-rpm.sh`

**Purpose:** Wrapper for RPM package building in Docker

**Arguments:**
- `$1`: Fedora version (e.g., 41)
- `$2`: Version (e.g., 0.2.1)
- `$3`: Output directory

**Process:**
1. Start container with fedora:$VERSION image
2. Mount source directory read-only
3. Install build dependencies
4. Setup RPM build tree
5. Run rpmbuild
6. Copy .rpm to output directory
7. Cleanup container

**Exit codes:**
- 0: Success
- 1: Build failed
- 2: Dependency installation failed

## Packaging Files Specifications

### `packaging/debian/control`

```
Source: plasma-desktop-organizer
Section: kde
Priority: optional
Maintainer: Joseph R. Quinn <email@example.com>
Build-Depends: debhelper-compat (= 13),
               cmake (>= 3.22),
               extra-cmake-modules (>= 6.0),
               qtbase6-dev,
               qtdeclarative6-dev,
               qtquickcontrols2-6-dev,
               libkf6coreaddons-dev,
               libkf6config-dev,
               libkf6kio-dev,
               libkf6iconthemes-dev,
               libkf6svg-dev,
               libkf6statusnotifieritem-dev,
               libkf6windowsystem-dev,
               libkf6kcmutils-dev,
               libkf6configwidgets-dev,
               libkf6dbusaddons-dev,
               liblayershellqt-dev (>= 6.0)
Standards-Version: 4.6.0
Homepage: https://github.com/quinnjr/plasma-desktop-organizer

Package: plasma-desktop-organizer
Architecture: amd64
Depends: ${shlibs:Depends}, ${misc:Depends}
Description: Fences-style desktop organizer for KDE Plasma
 Plasma Desktop Organizer provides customizable fence containers that overlay
 the native Plasma desktop, allowing organization of desktop items without
 replacing the desktop shell.
 .
 Features include drag-and-drop file management, KIO integration, live updates
 via KDirWatch, system tray control, and System Settings integration via KCM.
```

### `packaging/debian/rules`

```makefile
#!/usr/bin/make -f

%:
	dh $@ --buildsystem=cmake

override_dh_auto_configure:
	dh_auto_configure -- \
		-DCMAKE_INSTALL_PREFIX=/usr \
		-DCMAKE_BUILD_TYPE=Release \
		-DBUILD_TESTING=OFF

override_dh_auto_test:
	# Skip tests in package build (already tested in CI)
```

### `packaging/debian/changelog`

```
plasma-desktop-organizer (0.2.1-1) UNRELEASED; urgency=medium

  * New upstream release
  * System Settings (KCM) integration
  * D-Bus interface for IPC and service control
  * OrganizerSettings singleton for centralized configuration
  * Process control (start/stop/restart) from System Settings
  * Live configuration reload via D-Bus

 -- Joseph R. Quinn <email@example.com>  Wed, 16 Apr 2026 12:00:00 +0000
```

**Note:** Changelog is updated automatically during release process

### `packaging/debian/copyright`

```
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: plasma-desktop-organizer
Upstream-Contact: Joseph R. Quinn <email@example.com>
Source: https://github.com/quinnjr/plasma-desktop-organizer

Files: *
Copyright: 2026 Joseph R. Quinn
License: MIT

License: MIT
 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:
 .
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 .
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
```

### `packaging/debian/compat`

```
13
```

### `packaging/debian/source/format`

```
3.0 (quilt)
```

### `packaging/rpm/plasma-desktop-organizer.spec`

```spec
Name:           plasma-desktop-organizer
Version:        0.2.1
Release:        1%{?dist}
Summary:        Fences-style desktop organizer for KDE Plasma

License:        MIT
URL:            https://github.com/quinnjr/plasma-desktop-organizer
Source0:        %{name}-%{version}.tar.gz

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
%config(noreplace) %{_sysconfdir}/xdg/autostart/plasma-organizer.desktop

%changelog
* Wed Apr 16 2026 Joseph R. Quinn <email@example.com> - 0.2.1-1
- New upstream release
- System Settings (KCM) integration
- D-Bus interface for IPC and service control
- OrganizerSettings singleton for centralized configuration
- Process control from System Settings
- Live configuration reload via D-Bus
```

## Installation Instructions

### Ubuntu/Debian Installation

```bash
# Download the appropriate .deb for your distribution
wget https://github.com/quinnjr/plasma-desktop-organizer/releases/download/v0.2.1/plasma-desktop-organizer_0.2.1-1_amd64.deb

# Install package and dependencies
sudo dpkg -i plasma-desktop-organizer_0.2.1-1_amd64.deb
sudo apt-get install -f

# Launch application
plasma-organizer

# Or open System Settings
systemsettings6
# Navigate to: Workspace → Desktop Organizer
```

### Fedora Installation

```bash
# Download the appropriate .rpm for your Fedora version
wget https://github.com/quinnjr/plasma-desktop-organizer/releases/download/v0.2.1/plasma-desktop-organizer-0.2.1-1.fc41.x86_64.rpm

# Install package and dependencies
sudo dnf install plasma-desktop-organizer-0.2.1-1.fc41.x86_64.rpm

# Launch application
plasma-organizer

# Or open System Settings
systemsettings6
# Navigate to: Workspace → Desktop Organizer
```

### Uninstallation

**Ubuntu/Debian:**
```bash
sudo apt-get remove plasma-desktop-organizer
```

**Fedora:**
```bash
sudo dnf remove plasma-desktop-organizer
```

## Testing Plan

### Pre-Release Testing

**Test on each target distribution:**
1. Fresh VM or container with target distro
2. Install package
3. Verify binary runs: `plasma-organizer --version`
4. Verify KCM loads in System Settings
5. Create a fence and add files
6. Test D-Bus interface: `qdbus org.kde.plasma.desktoporganizer`
7. Test autostart integration
8. Uninstall and verify clean removal

### Automated Testing

**GitHub Actions test job** (separate from build):
- Install built package in clean container
- Run smoke tests
- Verify file installation paths
- Check D-Bus service activation

## Future Enhancements

**Phase 1 (Current):** GitHub releases with manual download
**Phase 2:** PPA for Ubuntu (launchpad.net)
**Phase 3:** Fedora COPR repository
**Phase 4:** Debian mentors / official Debian repository
**Phase 5:** OpenSUSE Build Service for wider distro support
**Phase 6:** ARM64 builds for Raspberry Pi / ARM desktops
**Phase 7:** Package signing with GPG keys

## Success Criteria

- [ ] Packages build successfully for all 5 distributions
- [ ] All packages install without errors on target distros
- [ ] Packages correctly declare dependencies
- [ ] Installed application runs and KCM loads
- [ ] Files install to correct system paths
- [ ] D-Bus service activates properly
- [ ] Packages can be cleanly uninstalled
- [ ] Build completes in < 15 minutes
- [ ] Release artifacts automatically uploaded to GitHub

## References

- [Debian Policy Manual](https://www.debian.org/doc/debian-policy/)
- [Debian New Maintainers' Guide](https://www.debian.org/doc/manuals/maint-guide/)
- [Fedora Packaging Guidelines](https://docs.fedoraproject.org/en-US/packaging-guidelines/)
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [CMake Packaging](https://cmake.org/cmake/help/latest/manual/cpack.1.html)
