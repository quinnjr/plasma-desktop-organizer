import QtQuick
import QtQuick.Controls
import dev.quinnjr.organizer 1.0

Item {
    id: fenceContainer

    property string fenceId: ""
    property string title: ""
    property bool rolledUp: false
    property int iconSize: 64
    property real fenceOpacity: 0.85

    property string selectedUrl: ""

    signal titleEdited(string fenceId, string newTitle)
    signal rollupToggled(string fenceId, bool newRolledUp)
    signal closeRequested(string fenceId)
    signal moveFinished(string fenceId, int newX, int newY)
    signal resizeFinished(string fenceId, int newX, int newY, int newWidth, int newHeight)
    signal iconClicked(string fenceId, string url, var mouse)
    signal iconDoubleClicked(string fenceId, string url)
    signal iconDragStarted(string fenceId, string url)
    signal iconDragMoved(string fenceId, string url, real sceneX, real sceneY)
    signal iconDragEnded(string fenceId, string url, real sceneX, real sceneY)

    function triggerRename() {
        titleBar.startEditing();
    }

    // Background
    Rectangle {
        anchors.fill: parent
        color: "#cc0d0d1a"
        opacity: fenceContainer.fenceOpacity
        radius: 6
        border.color: "#556666aa"
        border.width: 1
    }

    FenceFileModel {
        id: fenceFiles
        directory: fenceContainer.fenceId !== "" ? fenceManager.fenceDirectory(fenceContainer.fenceId) : ""
    }

    FenceTitleBar {
        id: titleBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        fenceId: fenceContainer.fenceId
        title: fenceContainer.title
        rolledUp: fenceContainer.rolledUp

        onDragMoved: function(dx, dy) {
            fenceContainer.x += dx;
            fenceContainer.y += dy;
        }
        onDragEnded: {
            fenceContainer.moveFinished(fenceContainer.fenceId,
                                        Math.round(fenceContainer.x),
                                        Math.round(fenceContainer.y));
        }
        onRollupClicked: fenceContainer.rollupToggled(fenceContainer.fenceId, !fenceContainer.rolledUp)
        onCloseClicked: fenceContainer.closeRequested(fenceContainer.fenceId)
        onTitleEdited: function(newTitle) {
            fenceContainer.titleEdited(fenceContainer.fenceId, newTitle);
        }
    }

    // Content area (hidden when rolled up)
    Item {
        id: contentArea
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        visible: !fenceContainer.rolledUp
        clip: true

        DropArea {
            anchors.fill: parent
            keys: ["text/uri-list"]

            onDropped: function(drop) {
                if (drop.hasUrls) {
                    var urls = drop.urls.map(function(u) { return u.toString(); });
                    fenceManager.moveUrlsToFence(urls, fenceContainer.fenceId);
                    drop.accept(Qt.MoveAction);
                }
            }

            FenceIconGrid {
                anchors.fill: parent
                fileModel: fenceFiles
                iconSize: fenceContainer.iconSize
                selectedUrl: fenceContainer.selectedUrl

                onIconClicked: function(url, mouse) {
                    if (mouse.button === Qt.RightButton) {
                        iconContextMenu.targetUrl = url;
                        iconContextMenu.popup();
                    } else {
                        fenceContainer.iconClicked(fenceContainer.fenceId, url, mouse);
                    }
                }
                onIconDoubleClicked: function(url) {
                    fenceContainer.iconDoubleClicked(fenceContainer.fenceId, url);
                }
                onIconDragStarted: function(url) {
                    fenceContainer.iconDragStarted(fenceContainer.fenceId, url);
                }
                onIconDragMoved: function(url, sx, sy) {
                    fenceContainer.iconDragMoved(fenceContainer.fenceId, url, sx, sy);
                }
                onIconDragEnded: function(url, sx, sy) {
                    fenceContainer.iconDragEnded(fenceContainer.fenceId, url, sx, sy);
                }
            }
        }
    }

    // Resize handle (bottom-right corner)
    MouseArea {
        id: resizeHandle
        width: 16; height: 16
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        cursorShape: Qt.SizeFDiagCursor
        visible: !fenceContainer.rolledUp

        property point pressScenePos
        property size pressSize

        onPressed: function(mouse) {
            // Capture in scene coordinates so the reference point
            // doesn't shift as the fence grows during the drag.
            var scene = mapToItem(null, mouse.x, mouse.y);
            pressScenePos = Qt.point(scene.x, scene.y);
            pressSize = Qt.size(fenceContainer.width, fenceContainer.height);
        }
        onPositionChanged: function(mouse) {
            if (pressed) {
                var scene = mapToItem(null, mouse.x, mouse.y);
                var newW = Math.max(200, pressSize.width + (scene.x - pressScenePos.x));
                var newH = Math.max(150, pressSize.height + (scene.y - pressScenePos.y));
                fenceContainer.width = newW;
                fenceContainer.height = newH;
            }
        }
        onReleased: {
            fenceContainer.resizeFinished(fenceContainer.fenceId,
                Math.round(fenceContainer.x), Math.round(fenceContainer.y),
                Math.round(fenceContainer.width), Math.round(fenceContainer.height));
        }

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "#44ffffff"
            border.width: 1
            radius: 2
        }
    }

    Behavior on height {
        NumberAnimation { duration: 150; easing.type: Easing.InOutQuad }
    }

    Menu {
        id: iconContextMenu
        property string targetUrl: ""

        MenuItem {
            text: "Open"
            onTriggered: Qt.openUrlExternally(iconContextMenu.targetUrl)
        }
        MenuItem {
            text: "Remove from Fence"
            onTriggered: {
                fenceManager.moveUrlsToDesktop([iconContextMenu.targetUrl]);
            }
        }
    }
}
