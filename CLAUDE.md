# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Configure
cmake -B build -DCMAKE_INSTALL_PREFIX=~/.local -DBUILD_TESTING=ON

# Build
cmake --build build --parallel

# Install
cmake --install build

# Clean build
rm -rf build && cmake -B build -DBUILD_TESTING=ON && cmake --build build --parallel
```

## Testing

```bash
# Run all tests
cd build && ctest --output-on-failure

# Run specific test suite
cd build && ./bin/test_fencemanager
cd build && ./bin/test_fencemodel
cd build && ./bin/test_fencefilemodel
cd build && ./bin/test_fencefilemodel_modifications

# Run all tests with verbose output
cd build && ctest -V

# Run specific test method (using QTest filter)
cd build && ./bin/test_fencemanager createFence_addsToList
```

## Coverage

```bash
# Build with coverage enabled
cmake -B build -DBUILD_TESTING=ON -DBUILD_COVERAGE=ON
cmake --build build --parallel

# Generate coverage report
cmake --build build --target coverage

# View report
xdg-open build/coverage_html/index.html
```

Current coverage: **94.4%** (337/357 lines). All business logic is covered; gaps are KIO async blocks (require live daemon), KDirWatch timing-sensitive events, and delegating constructors.

## Architecture

### Core Components

- **`Application`** (QGuiApplication subclass)
  - Entry point, owns the FenceManager singleton
  - Creates one `LayerShellWindow` per QScreen
  - Manages screenAdded/screenRemoved signals
  - Owns the KStatusNotifierItem system tray icon

- **`FenceManager`** (singleton QObject)
  - Owns all fences (stored in `QVector<Fence>`)
  - JSON persistence at `~/.local/share/dev.quinnjr.desktop-organizer/fences.json`
  - Per-fence directories at `~/.local/share/dev.quinnjr.desktop-organizer/fences/<uuid>/`
  - Signals: `fenceAdded(int)`, `fenceRemoved(int)`, `fenceUpdated(int)`
  - **Critical**: Emit `fenceRemoved` BEFORE `m_fences.removeAt()` (QAbstractItemModel contract)
  - **KIO operations**: `moveUrlsToFence()`, `moveUrlsToDesktop()` start async KIO::move jobs

- **`FenceModel`** (QAbstractListModel)
  - Exposes `FenceManager::fences()` to QML
  - Roles: FenceIdRole, TitleRole, ScreenRole, XRole, YRole, WidthRole, HeightRole, RolledUpRole, IconSizeRole
  - QML names: fenceId, title, screen, fenceX, fenceY, fenceWidth, fenceHeight, rolledUp, iconSize
  - `setData()` writes through to `FenceManager::updateFence()`
  - Constructor wires FenceManager signals to begin/endInsertRows, begin/endRemoveRows, dataChanged

- **`FenceFileModel`** (QAbstractListModel with KDirWatch)
  - Watches a directory with `KDirWatch` in `WatchFiles` mode
  - Roles: FileNameRole, FileUrlRole, IconNameRole, DisplayNameRole, IsDirRole
  - `.desktop` file cache: `QHash<QString, QPair<QString,QString>>` stores KDesktopFile Name=/Icon=
  - Cache populated in `refresh()` (sync) and `onCreated()`/`onDirty()` (async)
  - `setDirectory()` swaps watched directory; `directoryChanged()` signal emitted
  - **Has default constructor** for QML instantiation via `qmlRegisterType`

- **`LayerShellWindow`** (QObject, one per screen)
  - Owns `std::unique_ptr<QQuickView> m_view`
  - `setupLayerShell()` MUST call `show()` first, then `LayerShellQt::Window::get(m_view.get())`
  - Layer: `LAYER_BOTTOM` (between wallpaper and windows)
  - Anchors: all 4 edges, exclusiveZone(-1), KeyboardInteractivityOnDemand
  - `updateInputRegion()`: builds QRegion from fence rects, calls `m_view->setMask(region)`
  - Connected to all three FenceManager signals to keep input region in sync

### QML Structure

- **`qml/main.qml`** - Root scene
  - Repeater over `fenceModel` creates FenceContainer delegates
  - Draw-to-create: MouseArea z:-1, 10px deadzone, minimum 100×80 creates fence + triggers rename
  - Double-click anywhere toggles `fencesVisible`
  - Right-click Menu: "New Fence", "Hide/Show All Fences"

- **`qml/FenceContainer.qml`** - Per-fence component
  - Dark translucent background rect
  - FenceTitleBar (drag-to-move, inline rename, rollup/close buttons)
  - DropArea wrapping FenceIconGrid (receives dragged files)
  - Resize handle (bottom-right corner, uses scene coords via mapToItem(null, ...))
  - **Critical**: Owns a FenceFileModel instance bound to `fenceManager.fenceDirectory(fenceId)`

- **`qml/FenceTitleBar.qml`**
  - Drag area with delta tracking in scene coords
  - `startEditing()` public function for programmatic rename trigger
  - Inline rename TextField (activated on double-click or external call)
  - Rollup/close ToolButtons

- **`qml/FenceIconGrid.qml`**
  - Flickable > Grid with `columnCount: Math.floor(width/columnWidth)`
  - Uses `required property var model` pattern (avoids role-name shadowing)
  - Accesses roles via `model.fileUrl`, `model.fileName`, etc.

- **`qml/DesktopIcon.qml`**
  - Kirigami.Icon + two-line Text
  - `Drag.dragType: Drag.Automatic`
  - **Critical**: `Drag.active` managed explicitly (`iconRoot.Drag.active = true/false`)
  - `Drag.mimeData: ({ "text/uri-list": iconRoot.fileUrl + "\r\n" })`

### QML Registration

`Application::init()` calls:
```cpp
qmlRegisterType<FenceFileModel>("dev.quinnjr.organizer", 1, 0, "FenceFileModel");
```

This makes FenceFileModel instantiable in QML. The model is created per-fence with directory binding.

### Important Conventions

- **KDE Frameworks target names**: Use `KF6::KIOCore` (not `KF6::KIO`), `KF6::ConfigCore` (not `KF6::Config`)
- **QT_NO_KEYWORDS**: KDE compiler settings require `Q_SIGNALS:` / `Q_SLOTS:` not `signals:` / `slots:`
- **Singleton pattern**: `FenceManager::instance()` has `Q_ASSERT_X` null guard; must construct before calling
- **Input region coordinate space**: Fence rects must be in screen coords; resize handle uses `mapToItem(null, ...)` to avoid local-coord bugs
- **Drag.active lifecycle**: Must be managed explicitly (set true in onPositionChanged, false in onReleased before dragEnded) to avoid race with DropArea

### Test Structure

Tests use `QTemporaryDir` for isolation and `QStandardPaths::setTestModeEnabled(true)` to redirect paths to `~/.qttest/`.

- `test_fencemanager.cpp` - 20 tests covering CRUD, signals, JSON I/O, KIO URL validation
- `test_fencemodel.cpp` - 13 tests with QAbstractItemModelTester, setData, flags
- `test_fencefilemodel.cpp` - 14 tests for KDirWatch, .desktop cache, all data roles
- `test_fencefilemodel_modifications.cpp` - 3 tests for file modification detection (async KDirWatch events with `QSignalSpy::wait(2000)`)

**Signal spies**: Always attach before the triggering action. Use `.wait(2000)` for async KDirWatch events.

**QAbstractItemModelTester**: All model tests use `FailureReportingMode::Fatal` to catch contract violations immediately.

## Running the Application

```bash
# From build directory
QT_QPA_PLATFORM=wayland ./bin/plasma-organizer

# From installed location
QT_QPA_PLATFORM=wayland plasma-organizer
```

**Prerequisites**: Plasma desktop must use `org.kde.plasma.folder` for desktop containments (not the old dev.quinnjr.desktoporganizer QML package). Edit `~/.config/plasma-org.kde.plasma.desktop-appletsrc` if needed.

## CMake Structure

- **`organizer_core`** - static library with all business logic (application, fencemanager, fencemodel, fencefilemodel, layershellwindow)
- **`plasma-organizer`** - executable (main.cpp + QML resources via qt_add_resources)
- **PUBLIC deps** on organizer_core: Qt6::Core, Qt6::Gui, Qt6::Quick, KF6::CoreAddons (exposed in headers)
- **PRIVATE deps** on organizer_core: KF6::KIOCore, KF6::IconThemes, KF6::ConfigCore, KF6::StatusNotifierItem, Qt6::Qml, LayerShellQt::Interface
- Tests link against `organizer_core` static lib and have `--coverage` flags when `BUILD_COVERAGE=ON`

## Coverage Exclusions

- `src/application.cpp` / `src/layershellwindow.cpp` / `src/main.cpp` - require Wayland compositor
- KIO job blocks in `fencemanager.cpp` - marked with `LCOV_EXCL_START/STOP`, require live KIO scheduler
- `FenceFileModel::onDirty()` - marked with `LCOV_EXCL_LINE`, timing-sensitive KDirWatch events
- Default delegating constructor in `FenceFileModel` - gcov has trouble with constructor delegation
