# Changelog

All notable changes to this project will be documented in this file.

## [0.2.0] - 2026-04-16

### Added

- **Native package distribution** for Ubuntu, Debian, and Fedora
  - .deb packages for Ubuntu 24.04 LTS, Ubuntu 24.10, Debian 13
  - .rpm packages for Fedora 40, Fedora 41
  - Automated builds via GitHub Actions on tagged releases
  - Packages install to /usr for system-wide availability
- Debian packaging files following Debian Policy Manual
- RPM spec file following Fedora Packaging Guidelines
- GitHub Actions workflow with matrix builds for 5 distributions
- Automatic package upload to GitHub releases

### Changed

- Bumped project version to 0.2.0 in CMakeLists.txt

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0] - 2026-04-15

### Added

- Complete C++ rewrite of desktop organizer
- Full-screen wlr-layer-shell overlay using LayerShellQt at LAYER_BOTTOM
- Fence management: create, delete, rename, move, resize, roll-up
- Input region management for click-through to native desktop icons
- KIO-based file operations with undo support:
  - `moveUrlsToFence()` - move files into fence directories
  - `moveUrlsToDesktop()` - move files back to ~/Desktop
- KDirWatch live directory monitoring for real-time icon updates
- KDesktopFile parsing with metadata caching for .desktop files
- Qt Quick drag-drop with `Drag.Automatic` for cross-process operations
- KStatusNotifierItem system tray icon with:
  - "Hide/Show All Fences" toggle
  - "Quit" action
- QML UI components:
  - `main.qml` - root scene with draw-to-create and global hotkeys
  - `FenceContainer.qml` - fence background, title bar, icon grid, resize handle
  - `FenceTitleBar.qml` - drag-to-move, inline rename, rollup/close
  - `FenceIconGrid.qml` - responsive grid layout with Flickable
  - `DesktopIcon.qml` - icon + label with drag support
- JSON persistence at `~/.local/share/dev.quinnjr.desktop-organizer/fences.json`
- Per-fence file storage at `~/.local/share/dev.quinnjr.desktop-organizer/fences/<uuid>/`
- Autostart .desktop file at `~/.local/share/autostart/`
- Multi-screen support: one LayerShellWindow per QScreen
- QAbstractListModel implementations:
  - `FenceModel` - exposes fence list to QML
  - `FenceFileModel` - exposes directory contents with KFileItem metadata
- Comprehensive test suite (94.4% line coverage):
  - `test_fencemanager` - 20 tests covering CRUD, signals, JSON I/O, KIO guards
  - `test_fencemodel` - 13 tests covering QAbstractItemModel contract, setData, flags
  - `test_fencefilemodel` - 14 tests covering KDirWatch, .desktop cache, data roles
  - `test_fencefilemodel_modifications` - 3 tests for file modification detection
- CMake build system with:
  - `organizer_core` static library (all business logic)
  - `plasma-organizer` executable (main + QML resources)
  - Optional `BUILD_TESTING` for test suites
  - Optional `BUILD_COVERAGE` for lcov/genhtml reports
- Git-flow branching model with conventional commits
- Coverage infrastructure:
  - lcov 2.x integration with genhtml HTML reports
  - Automatic exclusion of system headers, moc files, Wayland sources
  - LCOV_EXCL annotations for untestable async KIO operations

### Technical Details

- **Language**: C++20
- **Qt**: 6.7+ (Core, Gui, Quick, QuickControls2, Qml)
- **KDE Frameworks**: 6.0+ (CoreAddons, Config, KIO, IconThemes, Svg, StatusNotifierItem, WindowSystem)
- **Protocol**: wlr-layer-shell via LayerShellQt::Interface
- **Build**: CMake 3.22+, QT_AUTOMOC enabled
- **Tests**: QTest + QAbstractItemModelTester
- **Coverage**: gcov/llvm-cov + lcov 2.4

### Architecture

- Singleton `FenceManager` owns all fences and handles persistence
- `Application` creates one `LayerShellWindow` per screen
- Each window hosts a `QQuickView` with full-screen QML scene
- Input regions built from fence geometries via `QWindow::setMask(QRegion)`
- File operations are async KIO jobs; signals used for completion
- Directory watching is async KDirWatch with `WatchFiles` mode
- Models connect to manager signals for automatic view updates

[0.1.0]: https://github.com/quinnjr/plasma-desktop-organizer/releases/tag/v0.1.0
