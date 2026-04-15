import QtQuick
import QtQuick.Layouts
import QtCore
import QtQml.Models
import Qt.labs.folderlistmodel
import org.kde.plasma.plasmoid
import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

import "lib/FenceModel.js" as FenceModel
import "lib/IconHelper.js" as IconHelper

ContainmentItem {
    id: root

    property var fences: []
    property bool fencesVisible: true
    property string selectedIconUrl: ""
    property int maxZ: 1
    property var desktopFilesList: []

    // --- Desktop folder model ---
    FolderListModel {
        id: desktopFolderModel
        folder: {
            var loc = StandardPaths.writableLocation(StandardPaths.DesktopLocation).toString();
            if (loc.startsWith("file://")) return loc;
            return "file://" + loc;
        }
        showDirs: true
        showDotAndDotDot: false
        sortField: FolderListModel.Name
    }

    // Mirror FolderListModel into a JS array via Instantiator
    Instantiator {
        id: fileInstantiator
        model: desktopFolderModel
        delegate: QtObject {
            required property string fileName
            required property url fileUrl
            required property bool fileIsDir
        }
        onObjectAdded: rebuildTimer.restart()
        onObjectRemoved: rebuildTimer.restart()
    }

    Timer {
        id: rebuildTimer
        interval: 150
        onTriggered: refreshDesktopFiles()
    }

    function refreshDesktopFiles() {
        var files = [];
        for (var i = 0; i < fileInstantiator.count; i++) {
            var obj = fileInstantiator.objectAt(i);
            if (obj) {
                files.push({
                    url: obj.fileUrl.toString(),
                    fileName: obj.fileName,
                    isDir: obj.fileIsDir
                });
            }
        }
        desktopFilesList = files;
        reconcileDeletedFiles();
    }

    function reconcileDeletedFiles() {
        var currentUrls = desktopFilesList.map(function(f) { return f.url; });
        FenceModel.removeDeletedFiles(fences, currentUrls);
        persistFences();
    }

    // --- Fence lifecycle ---
    Component.onCompleted: {
        fences = FenceModel.loadFences(Plasmoid.configuration.fencesData);
        fences = fences.slice();
    }

    function persistFences() {
        Plasmoid.configuration.fencesData = FenceModel.saveFences(fences);
        fences = fences.slice(); // new array ref triggers Repeater update
    }

    function unfencedFiles() {
        var assigned = FenceModel.allAssignedUrls(fences);
        return desktopFilesList.filter(function(f) { return !assigned[f.url]; });
    }

    function enrichFenceIcons(fenceIcons) {
        var fileMap = {};
        for (var i = 0; i < desktopFilesList.length; i++) {
            fileMap[desktopFilesList[i].url] = desktopFilesList[i];
        }
        var result = [];
        for (var j = 0; j < fenceIcons.length; j++) {
            var icon = fenceIcons[j];
            var file = fileMap[icon.url];
            result.push({
                url: icon.url,
                fileName: file ? file.fileName : icon.url.split('/').pop(),
                isDir: file ? file.isDir : false,
                col: icon.col,
                row: icon.row
            });
        }
        return result;
    }

    function hitTestFence(px, py) {
        var best = null;
        var bestZ = -1;
        for (var i = 0; i < fenceRepeater.count; i++) {
            var item = fenceRepeater.itemAt(i);
            if (item && item.opacity > 0 && item.enabled && !item.rolledUp &&
                px >= item.x && px <= item.x + item.width &&
                py >= item.y && py <= item.y + item.height &&
                item.z > bestZ) {
                best = item;
                bestZ = item.z;
            }
        }
        return best ? FenceModel.findFence(fences, best.fenceId) : null;
    }

    // --- Unfenced icons (flow layout behind fences) ---
    Flow {
        id: unfencedFlow
        anchors.fill: parent
        anchors.margins: 16
        spacing: 8
        z: 0

        Repeater {
            model: root.unfencedFiles()

            DesktopIcon {
                required property var modelData
                required property int index

                fileUrl: modelData.url
                fileName: modelData.fileName
                fileIsDir: modelData.isDir
                iconSize: Plasmoid.configuration.iconSize
                selected: fileUrl === root.selectedIconUrl

                onClicked: function(mouse) {
                    if (mouse.button === Qt.RightButton) {
                        iconMenu.iconUrl = fileUrl;
                        iconMenu.inFenceId = "";
                        iconMenu.popup();
                    } else {
                        root.selectedIconUrl = fileUrl;
                    }
                }
                onDoubleClicked: Qt.openUrlExternally(fileUrl)
                onDragStarted: root.beginIconDrag(fileUrl, "")
                onDragMoved: function(sx, sy) { root.updateIconDrag(sx, sy) }
                onDragEnded: function(sx, sy) { root.endIconDrag(sx, sy) }
            }
        }
    }

    // --- Fence containers ---
    Repeater {
        id: fenceRepeater
        model: root.fences

        FenceContainer {
            required property var modelData
            required property int index

            fenceId: modelData.id
            title: modelData.title
            icons: root.enrichFenceIcons(modelData.icons)
            iconSize: modelData.iconSize || 64
            rolledUp: modelData.rolledUp || false
            initialHeight: modelData.height
            fenceOpacity: Plasmoid.configuration.fenceOpacity / 100
            animationSpeed: Plasmoid.configuration.animationSpeed

            x: modelData.x
            y: modelData.y
            width: modelData.width
            z: 1

            opacity: root.fencesVisible ? 1 : 0
            scale: root.fencesVisible ? 1 : 0.95
            enabled: root.fencesVisible

            Behavior on opacity {
                NumberAnimation { duration: Plasmoid.configuration.animationSpeed; easing.type: Easing.InOutQuad }
            }
            Behavior on scale {
                NumberAnimation { duration: Plasmoid.configuration.animationSpeed; easing.type: Easing.InOutQuad }
            }

            onTitleEdited: function(fid, newTitle) {
                var f = FenceModel.findFence(root.fences, fid);
                if (f) f.title = newTitle;
                root.persistFences();
            }
            onRollupToggled: function(fid) {
                var f = FenceModel.findFence(root.fences, fid);
                if (f) f.rolledUp = !f.rolledUp;
                root.persistFences();
            }
            onCloseRequested: function(fid) {
                FenceModel.deleteFence(root.fences, fid);
                root.persistFences();
            }
            onMoveFinished: function(fid) {
                var f = FenceModel.findFence(root.fences, fid);
                if (f) { f.x = x; f.y = y; }
                root.persistFences();
            }
            onResizeFinished: function(fid) {
                var f = FenceModel.findFence(root.fences, fid);
                if (f) { f.x = x; f.y = y; f.width = width; f.height = height; }
                root.persistFences();
            }
            onIconClicked: function(fid, url, mouse) {
                if (mouse.button === Qt.RightButton) {
                    iconMenu.iconUrl = url;
                    iconMenu.inFenceId = fid;
                    iconMenu.popup();
                } else {
                    root.selectedIconUrl = url;
                }
            }
            onIconDoubleClicked: function(fid, url) {
                Qt.openUrlExternally(url);
            }
            onIconDragStarted: function(fid, url) {
                root.beginIconDrag(url, fid);
            }
            onIconDragMoved: function(fid, url, sx, sy) {
                root.updateIconDrag(sx, sy);
            }
            onIconDragEnded: function(fid, url, sx, sy) {
                root.endIconDrag(sx, sy);
            }
            onActivated: function(fid) {
                root.maxZ++;
                z = root.maxZ;
            }
            onContextMenuRequested: function(fid) {
                fenceMenu.targetFenceId = fid;
                fenceMenu.popup();
            }
        }
    }

    // --- Drag state ---
    property bool isDraggingIcon: false
    property string dragIconUrl: ""
    property string dragSourceFenceId: ""

    function beginIconDrag(url, sourceFenceId) {
        dragIconUrl = url;
        dragSourceFenceId = sourceFenceId;
        isDraggingIcon = true;
    }

    function updateIconDrag(sceneX, sceneY) {
        var local = root.mapFromItem(null, sceneX, sceneY);
        dragProxy.x = local.x - 24;
        dragProxy.y = local.y - 24;
    }

    function endIconDrag(sceneX, sceneY) {
        var local = root.mapFromItem(null, sceneX, sceneY);
        var targetFence = hitTestFence(local.x, local.y);
        FenceModel.unassignIconFromAll(fences, dragIconUrl);

        if (targetFence) {
            var cell = FenceModel.nextEmptyCell(targetFence);
            FenceModel.assignIcon(fences, targetFence.id, dragIconUrl, cell.col, cell.row);
        }

        isDraggingIcon = false;
        dragIconUrl = "";
        dragSourceFenceId = "";
        persistFences();
    }

    // Drag proxy (floating icon that follows mouse during drag)
    Kirigami.Icon {
        id: dragProxy
        visible: root.isDraggingIcon
        width: 48; height: 48
        opacity: 0.8
        z: 10001
        source: {
            if (!root.dragIconUrl) return "";
            var name = root.dragIconUrl.split('/').pop();
            var isDir = false;
            for (var i = 0; i < root.desktopFilesList.length; i++) {
                if (root.desktopFilesList[i].url === root.dragIconUrl) {
                    isDir = root.desktopFilesList[i].isDir;
                    break;
                }
            }
            return IconHelper.iconForFile(name, isDir);
        }
    }

    SelectionRectangle {
        id: selectionRect
        z: 9999
    }

    // --- Desktop gestures ---
    MouseArea {
        id: desktopMouse
        anchors.fill: parent
        z: -1
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        property bool drawing: false
        property point pressPos

        onPressed: function(mouse) {
            console.log("DESKTOP PRESSED at", mouse.x, mouse.y, "root size:", root.width, root.height, "flow size:", unfencedFlow.width, unfencedFlow.height, "icons:", unfencedFlow.children.length);
            pressPos = Qt.point(mouse.x, mouse.y);
            drawing = false;

            if (mouse.button === Qt.LeftButton) {
                root.selectedIconUrl = "";
            }
            if (mouse.button === Qt.RightButton) {
                desktopMenu.popup();
            }
        }

        onPositionChanged: function(mouse) {
            if (pressed && !(mouse.buttons & Qt.RightButton)) {
                if (!drawing) {
                    var dx = mouse.x - pressPos.x;
                    var dy = mouse.y - pressPos.y;
                    if (Math.sqrt(dx * dx + dy * dy) > 10) {
                        drawing = true;
                        selectionRect.begin(pressPos);
                    }
                }
                if (drawing) {
                    selectionRect.update(Qt.point(mouse.x, mouse.y));
                }
            }
        }

        onReleased: function(mouse) {
            if (drawing) {
                var rect = selectionRect.finish();
                drawing = false;
                if (rect.width >= 100 && rect.height >= 80) {
                    var newFence = FenceModel.createFence(root.fences, rect.x, rect.y,
                                           rect.width, rect.height,
                                           Plasmoid.configuration.defaultFenceTitle,
                                           Plasmoid.configuration.iconSize);
                    root.persistFences();
                    // Trigger inline rename on the new fence after Repeater instantiates it
                    var newId = newFence.id;
                    Qt.callLater(function() {
                        for (var i = 0; i < fenceRepeater.count; i++) {
                            var item = fenceRepeater.itemAt(i);
                            if (item && item.fenceId === newId) {
                                item.triggerRename();
                                break;
                            }
                        }
                    });
                }
            }
        }

        onDoubleClicked: function(mouse) {
            if (mouse.button === Qt.LeftButton && !drawing && Plasmoid.configuration.doubleClickToHide) {
                root.fencesVisible = !root.fencesVisible;
            }
        }
    }

    // --- Context menus ---
    PlasmaComponents.Menu {
        id: desktopMenu

        PlasmaComponents.MenuItem {
            text: "New Fence"
            icon.name: "list-add"
            onClicked: {
                FenceModel.createFence(root.fences, 100, 100, 400, 300,
                    Plasmoid.configuration.defaultFenceTitle,
                    Plasmoid.configuration.iconSize);
                root.persistFences();
            }
        }
        PlasmaComponents.MenuItem {
            text: "Show All Fences"
            icon.name: "view-visible"
            enabled: !root.fencesVisible
            onClicked: root.fencesVisible = true
        }
        PlasmaComponents.MenuItem {
            text: "Hide All Fences"
            icon.name: "view-hidden"
            enabled: root.fencesVisible
            onClicked: root.fencesVisible = false
        }
    }

    PlasmaComponents.Menu {
        id: iconMenu
        property string iconUrl: ""
        property string inFenceId: ""

        PlasmaComponents.MenuItem {
            text: "Open"
            icon.name: "document-open"
            onClicked: Qt.openUrlExternally(iconMenu.iconUrl)
        }
        PlasmaComponents.MenuItem {
            text: "Remove from Fence"
            icon.name: "edit-delete-remove"
            visible: iconMenu.inFenceId !== ""
            onClicked: {
                FenceModel.unassignIconFromAll(root.fences, iconMenu.iconUrl);
                root.persistFences();
            }
        }
    }

    PlasmaComponents.Menu {
        id: fenceMenu
        property string targetFenceId: ""

        PlasmaComponents.MenuItem {
            text: "Rename"
            icon.name: "edit-rename"
            onClicked: {
                for (var i = 0; i < fenceRepeater.count; i++) {
                    var item = fenceRepeater.itemAt(i);
                    if (item && item.fenceId === fenceMenu.targetFenceId) {
                        item.triggerRename();
                        break;
                    }
                }
            }
        }
        PlasmaComponents.MenuItem {
            text: "Delete Fence"
            icon.name: "edit-delete"
            onClicked: {
                FenceModel.deleteFence(root.fences, fenceMenu.targetFenceId);
                root.persistFences();
            }
        }
    }
}
