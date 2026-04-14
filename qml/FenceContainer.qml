import QtQuick
import QtQuick.Controls

Item {
    id: fenceContainer

    property string fenceId: ""
    property string title: ""
    property bool rolledUp: false
    property int iconSize: 64
    property real fenceOpacity: 0.85

    signal titleEdited(string fenceId, string newTitle)
    signal rollupToggled(string fenceId)
    signal closeRequested(string fenceId)
    signal moveFinished(string fenceId, int newX, int newY)
    signal resizeFinished(string fenceId, int newX, int newY, int newWidth, int newHeight)
    signal activated(string fenceId)

    function triggerRename() {
        titleBar.titleLabel.startEditing();
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
            fenceContainer.activated(fenceContainer.fenceId);
        }
        onRollupClicked: fenceContainer.rollupToggled(fenceContainer.fenceId)
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
    }

    // Resize handle (bottom-right corner)
    MouseArea {
        id: resizeHandle
        width: 16; height: 16
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        cursorShape: Qt.SizeFDiagCursor
        visible: !fenceContainer.rolledUp

        property point pressPos
        property size pressSize

        onPressed: function(mouse) {
            pressPos = Qt.point(mouse.x, mouse.y);
            pressSize = Qt.size(fenceContainer.width, fenceContainer.height);
            fenceContainer.activated(fenceContainer.fenceId);
        }
        onPositionChanged: function(mouse) {
            if (pressed) {
                var newW = Math.max(200, pressSize.width + (mouse.x - pressPos.x));
                var newH = Math.max(150, pressSize.height + (mouse.y - pressPos.y));
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
}
