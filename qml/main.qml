import QtQuick
import QtQuick.Controls

Item {
    id: root
    anchors.fill: parent

    property bool fencesVisible: true
    property string selectedUrl: ""

    // Draw-to-create state
    property bool isDrawing: false
    property point drawStart
    property point drawCurrent

    // Selection rectangle overlay
    Rectangle {
        id: selectionRect
        visible: root.isDrawing
        color: "#2200aaff"
        border.color: "#aa00aaff"
        border.width: 1
        z: 9998

        x: Math.min(root.drawStart.x, root.drawCurrent.x)
        y: Math.min(root.drawStart.y, root.drawCurrent.y)
        width: Math.abs(root.drawCurrent.x - root.drawStart.x)
        height: Math.abs(root.drawCurrent.y - root.drawStart.y)
    }

    // Fence delegates
    Repeater {
        id: fenceRepeater
        model: fenceModel

        delegate: FenceContainer {
            required property string fenceId
            required property string title
            required property int fenceX
            required property int fenceY
            required property int fenceWidth
            required property int fenceHeight
            required property bool rolledUp
            required property int iconSize

            x: fenceX
            y: fenceY
            width: fenceWidth
            height: rolledUp ? 28 : fenceHeight

            fenceId: fenceId
            title: title
            rolledUp: rolledUp
            iconSize: iconSize
            selectedUrl: root.selectedUrl

            opacity: root.fencesVisible ? 1.0 : 0.0
            Behavior on opacity {
                NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
            }

            onTitleEdited: function(fid, newTitle) {
                fenceManager.renameFence(fid, newTitle);
            }
            onRollupToggled: function(fid, newRolledUp) {
                fenceManager.setRolledUp(fid, newRolledUp);
            }
            onCloseRequested: function(fid) {
                fenceManager.deleteFence(fid);
            }
            onMoveFinished: function(fid, nx, ny) {
                fenceManager.moveFence(fid, nx, ny);
            }
            onResizeFinished: function(fid, nx, ny, nw, nh) {
                fenceManager.resizeFence(fid, nx, ny, nw, nh);
            }
            onIconClicked: function(fid, url, mouse) {
                root.selectedUrl = url;
            }
            onIconDoubleClicked: function(fid, url) {
                Qt.openUrlExternally(url);
            }
        }
    }

    // Desktop mouse area — behind fences (z: -1), catches empty-desktop events
    MouseArea {
        id: desktopMouse
        anchors.fill: parent
        z: -1
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPressed: function(mouse) {
            if (mouse.button === Qt.LeftButton) {
                root.selectedUrl = "";
                root.isDrawing = false;
                root.drawStart = Qt.point(mouse.x, mouse.y);
                root.drawCurrent = Qt.point(mouse.x, mouse.y);
            } else if (mouse.button === Qt.RightButton) {
                desktopMenu.popup();
            }
        }
        onPositionChanged: function(mouse) {
            if (pressed && (mouse.buttons & Qt.LeftButton)) {
                root.drawCurrent = Qt.point(mouse.x, mouse.y);
                var dx = mouse.x - root.drawStart.x;
                var dy = mouse.y - root.drawStart.y;
                if (!root.isDrawing && Math.sqrt(dx*dx + dy*dy) > 10) {
                    root.isDrawing = true;
                }
            }
        }
        onReleased: function(mouse) {
            if (root.isDrawing) {
                root.isDrawing = false;
                var x = Math.min(root.drawStart.x, root.drawCurrent.x);
                var y = Math.min(root.drawStart.y, root.drawCurrent.y);
                var w = Math.abs(root.drawCurrent.x - root.drawStart.x);
                var h = Math.abs(root.drawCurrent.y - root.drawStart.y);
                if (w >= 100 && h >= 80) {
                    var newId = fenceManager.createFenceOnScreen(
                        screenName, x, y, w, h, "New Fence");
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
            if (mouse.button === Qt.LeftButton && !root.isDrawing) {
                root.fencesVisible = !root.fencesVisible;
            }
        }
    }

    // Desktop right-click menu
    Menu {
        id: desktopMenu
        MenuItem {
            text: "New Fence"
            onTriggered: {
                var id = fenceManager.createFenceOnScreen(
                    screenName, 100, 100, 400, 300, "New Fence");
                Qt.callLater(function() {
                    for (var i = 0; i < fenceRepeater.count; i++) {
                        var item = fenceRepeater.itemAt(i);
                        if (item && item.fenceId === id) {
                            item.triggerRename();
                            break;
                        }
                    }
                });
            }
        }
        MenuItem {
            text: root.fencesVisible ? "Hide All Fences" : "Show All Fences"
            onTriggered: root.fencesVisible = !root.fencesVisible
        }
    }
}
