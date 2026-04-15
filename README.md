# Plasma Desktop Organizer

A Fences-style desktop organizer for KDE Plasma that overlays the native desktop with customizable fence containers.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Features

- **Full-screen overlay**: Uses `wlr-layer-shell` to overlay the native Plasma desktop without replacing it
- **Fence management**: Create, resize, move, rename, and roll-up fence containers
- **Drag & drop**: Move files between fences and the desktop using Qt drag-drop
- **KIO integration**: File operations with undo support and progress reporting
- **Live updates**: KDirWatch monitors fence directories for real-time icon updates
- **Desktop file support**: Parses `.desktop` files for proper application names and icons
- **System tray**: KStatusNotifierItem provides hide/show toggle and quit menu
- **Persistence**: JSON-based configuration with automatic saving
- **Autostart**: Launches automatically with KDE session
- **Input regions**: Click-through on non-fence areas to interact with native desktop icons
- **Multi-screen**: Automatically creates one overlay per screen

## Requirements

- **CMake** 3.22+
- **Qt6** (Core, Gui, Quick, QuickControls2, Qml)
- **KDE Frameworks 6**:
  - CoreAddons
  - Config
  - KIO
  - IconThemes
  - Svg
  - StatusNotifierItem
  - WindowSystem
- **LayerShellQt** (wlr-layer-shell protocol)
- **C++20** compiler (GCC 10+, Clang 12+)

### Optional (for development):

- **lcov** (for coverage reports)
- **Qt6Test** (for running tests)

## Building

```bash
# Configure
cmake -B build -DCMAKE_INSTALL_PREFIX=~/.local -DBUILD_TESTING=ON

# Build
cmake --build build --parallel

# Run tests
cd build && ctest --output-on-failure

# Install
cmake --install build
```

### Coverage Reports

```bash
# Install lcov
sudo pacman -S lcov  # Arch Linux
# or: sudo apt install lcov  # Debian/Ubuntu

# Build with coverage
cmake -B build -DBUILD_TESTING=ON -DBUILD_COVERAGE=ON
cmake --build build --parallel

# Generate report
cmake --build build --target coverage

# Open report
xdg-open build/coverage_html/index.html
```

Current coverage: **94.4%** (337/357 lines)

## Usage

### First Run

1. **Revert Plasma desktop to native**:
   ```bash
   # Edit ~/.config/plasma-org.kde.plasma.desktop-appletsrc
   # Change containment plugin= lines to: plugin=org.kde.plasma.folder
   ```

2. **Start the organizer**:
   ```bash
   QT_QPA_PLATFORM=wayland plasma-organizer
   ```

3. **Create a fence**:
   - Right-click on desktop → "New Fence"
   - Or draw a rectangle (click and drag with 10px deadzone, minimum 100×80)

### Fence Operations

- **Move**: Drag the title bar
- **Resize**: Drag the bottom-right corner handle
- **Rename**: Double-click the title (or right-click → "Edit Name")
- **Roll up/down**: Click the roll-up button in title bar
- **Delete**: Click the close button
- **Hide all**: System tray → "Hide All Fences"

### File Operations

- **Add to fence**: Drag files from Dolphin or desktop onto a fence
- **Remove from fence**: Right-click icon → "Remove from Fence"
- **Open**: Double-click icon

Files are stored in `~/.local/share/dev.quinnjr.desktop-organizer/fences/<uuid>/`

## Architecture

```
plasma-organizer (executable)
└── organizer_core (static library)
    ├── Application         # QGuiApplication subclass, screen management
    ├── FenceManager        # Singleton, manages all fences, JSON persistence
    ├── FenceModel          # QAbstractListModel for QML
    ├── FenceFileModel      # QAbstractListModel with KDirWatch
    └── LayerShellWindow    # One per screen, owns QQuickView + LayerShellQt
```

QML components: `main.qml`, `FenceContainer.qml`, `FenceTitleBar.qml`, `FenceIconGrid.qml`, `DesktopIcon.qml`

## Testing

The project includes comprehensive unit tests with 94.4% line coverage:

- `test_fencemanager` (20 tests) - JSON persistence, CRUD operations, KIO guards
- `test_fencemodel` (13 tests) - QAbstractListModel contract, setData, flags
- `test_fencefilemodel` (14 tests) - KDirWatch, .desktop cache, data roles
- `test_fencefilemodel_modifications` (3 tests) - File modification detection

Run tests:
```bash
cd build
ctest --output-on-failure
```

## Configuration

Fences are stored in: `~/.local/share/dev.quinnjr.desktop-organizer/fences.json`

Autostart file: `~/.local/share/autostart/plasma-organizer.desktop`

## Troubleshooting

**Overlay doesn't appear**: Ensure `QT_QPA_PLATFORM=wayland` is set and your compositor supports `wlr-layer-shell`.

**Can't click desktop icons**: Input regions are calculated from fence positions. Check that fences aren't overlapping the icons.

**Fences don't persist**: Check permissions on `~/.local/share/dev.quinnjr.desktop-organizer/`.

**Autostart not working**: Verify `~/.local/share/autostart/plasma-organizer.desktop` exists and `X-KDE-autostart-enabled=true` is set.

## Development

This project follows [git-flow](https://nvie.com/posts/a-successful-git-branching-model/):

- `main` - stable releases only
- `develop` - integration branch
- `feature/*` - new features
- `release/*` - release preparation
- `hotfix/*` - production fixes

Commit messages follow [Conventional Commits](https://www.conventionalcommits.org/).

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by Stardock Fences
- Built with KDE Frameworks 6 and Qt6
- Uses LayerShellQt for Wayland layer-shell protocol
