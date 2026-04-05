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
            required property url fileURL
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
                    url: obj.fileURL.toString(),
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
                iconSize: 64
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

            x: modelData.x
            y: modelData.y
            width: modelData.width
            height: modelData.height
            z: 1

            opacity: root.fencesVisible ? 1 : 0
            scale: root.fencesVisible ? 1 : 0.95
            enabled: root.fencesVisible

            Behavior on opacity {
                NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
            }
            Behavior on scale {
                NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
            }

            onTitleChanged: function(fid, newTitle) {
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
            onActivated: function(fid) {
                root.maxZ++;
                z = root.maxZ;
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

    function finishIconDrop(dropX, dropY) {
        var targetFence = hitTestFence(dropX, dropY);
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
        source: root.dragIconUrl
            ? IconHelper.iconForFile(root.dragIconUrl.split('/').pop(), false)
            : ""
    }

    // Drag overlay (captures all mouse events during icon drag)
    MouseArea {
        id: dragOverlay
        anchors.fill: parent
        z: 10000
        visible: root.isDraggingIcon
        cursorShape: Qt.ClosedHandCursor

        onPositionChanged: function(mouse) {
            dragProxy.x = mouse.x - 24;
            dragProxy.y = mouse.y - 24;
        }

        onReleased: function(mouse) {
            root.finishIconDrop(mouse.x, mouse.y);
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
            pressPos = Qt.point(mouse.x, mouse.y);
            drawing = false;

            if (mouse.button === Qt.LeftButton) {
                root.selectedIconUrl = "";
            }
        }

        onPositionChanged: function(mouse) {
            if (pressed && mouse.button !== Qt.RightButton) {
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
                    FenceModel.createFence(root.fences, rect.x, rect.y,
                                           rect.width, rect.height, "New Fence");
                    root.persistFences();
                }
            }
        }

        onDoubleClicked: function(mouse) {
            if (mouse.button === Qt.LeftButton && !drawing) {
                root.fencesVisible = !root.fencesVisible;
            }
        }
    }

    // --- Placeholder context menus (wired up in Task 12) ---
    PlasmaComponents.Menu {
        id: iconMenu
        property string iconUrl: ""
        property string inFenceId: ""
    }
}
