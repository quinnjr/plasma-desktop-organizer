import QtQuick
import org.kde.ksvg as KSvg
import org.kde.kirigami as Kirigami

Item {
    id: fence

    required property string fenceId
    required property string title
    required property var icons
    required property int iconSize
    required property bool rolledUp
    required property real initialHeight

    property int titleBarHeight: 28
    property int handleSize: 6
    property int minWidth: 200
    property int minHeight: 150
    property real expandedHeight: initialHeight
    property real fenceOpacity: 0.8
    property int animationSpeed: 200

    signal titleEdited(string fenceId, string newTitle)
    signal rollupToggled(string fenceId)
    signal closeRequested(string fenceId)
    signal positionMoved(string fenceId, real newX, real newY)
    signal moveFinished(string fenceId)
    signal sizeEdited(string fenceId, real newX, real newY, real newW, real newH)
    signal resizeFinished(string fenceId)
    signal iconClicked(string fenceId, string url, var mouse)
    signal iconDoubleClicked(string fenceId, string url)
    signal iconDragStarted(string fenceId, string url)
    signal iconDragMoved(string fenceId, string url, real sceneX, real sceneY)
    signal iconDragEnded(string fenceId, string url, real sceneX, real sceneY)
    signal activated(string fenceId)
    signal contextMenuRequested(string fenceId)

    // Rollup animation
    implicitHeight: rolledUp ? titleBarHeight + background.margins.top + background.margins.bottom
                             : expandedHeight

    Behavior on implicitHeight {
        NumberAnimation { duration: fence.animationSpeed; easing.type: Easing.InOutQuad }
    }

    height: implicitHeight

    // Background
    KSvg.FrameSvgItem {
        id: background
        anchors.fill: parent
        imagePath: "widgets/background"
        enabledBorders: KSvg.FrameSvgItem.AllBorders
        opacity: fence.fenceOpacity
    }

    // Click to activate / right-click for context menu
    MouseArea {
        anchors.fill: parent
        z: -1
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: function(mouse) {
            fence.activated(fence.fenceId);
            if (mouse.button === Qt.RightButton) {
                fence.contextMenuRequested(fence.fenceId);
            }
            mouse.accepted = (mouse.button === Qt.RightButton);
        }
    }

    // Title bar
    FenceTitleBar {
        id: titleBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: background.margins.top
        anchors.leftMargin: background.margins.left
        anchors.rightMargin: background.margins.right
        title: fence.title
        rolledUp: fence.rolledUp

        onTitleEdited: function(newTitle) {
            fence.titleEdited(fence.fenceId, newTitle);
        }
        onRollupToggled: fence.rollupToggled(fence.fenceId)
        onCloseClicked: fence.closeRequested(fence.fenceId)
        onDragBy: function(dx, dy) {
            fence.x += dx;
            fence.y += dy;
            fence.positionMoved(fence.fenceId, fence.x, fence.y);
        }
        onDragFinished: fence.moveFinished(fence.fenceId)
    }

    // Icon grid (hidden when rolled up)
    IconGrid {
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: background.margins.left
        anchors.rightMargin: background.margins.right
        anchors.bottomMargin: background.margins.bottom
        anchors.topMargin: 4
        visible: !fence.rolledUp
        icons: fence.icons
        iconSize: fence.iconSize

        onIconClicked: function(url, mouse) {
            fence.iconClicked(fence.fenceId, url, mouse);
        }
        onIconDoubleClicked: function(url) {
            fence.iconDoubleClicked(fence.fenceId, url);
        }
        onIconDragStarted: function(url) {
            fence.iconDragStarted(fence.fenceId, url);
        }
        onIconDragMoved: function(url, sx, sy) {
            fence.iconDragMoved(fence.fenceId, url, sx, sy);
        }
        onIconDragEnded: function(url, sx, sy) {
            fence.iconDragEnded(fence.fenceId, url, sx, sy);
        }
    }

    // --- Resize handles (hidden when rolled up) ---

    // Edges
    ResizeHandle {
        anchors.left: parent.left; anchors.top: parent.top; anchors.bottom: parent.bottom
        width: handleSize; edges: 1; visible: !fence.rolledUp
        onResized: function(dx, dy, dw, dh) { applyResize(dx, dy, dw, dh) }
        onResizeFinished: fence.resizeFinished(fence.fenceId)
    }
    ResizeHandle {
        anchors.right: parent.right; anchors.top: parent.top; anchors.bottom: parent.bottom
        width: handleSize; edges: 2; visible: !fence.rolledUp
        onResized: function(dx, dy, dw, dh) { applyResize(dx, dy, dw, dh) }
        onResizeFinished: fence.resizeFinished(fence.fenceId)
    }
    ResizeHandle {
        anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
        height: handleSize; edges: 4; visible: !fence.rolledUp
        onResized: function(dx, dy, dw, dh) { applyResize(dx, dy, dw, dh) }
        onResizeFinished: fence.resizeFinished(fence.fenceId)
    }
    ResizeHandle {
        anchors.bottom: parent.bottom; anchors.left: parent.left; anchors.right: parent.right
        height: handleSize; edges: 8; visible: !fence.rolledUp
        onResized: function(dx, dy, dw, dh) { applyResize(dx, dy, dw, dh) }
        onResizeFinished: fence.resizeFinished(fence.fenceId)
    }

    // Corners
    ResizeHandle {
        anchors.left: parent.left; anchors.top: parent.top
        width: handleSize * 2; height: handleSize * 2; edges: 5; visible: !fence.rolledUp
        onResized: function(dx, dy, dw, dh) { applyResize(dx, dy, dw, dh) }
        onResizeFinished: fence.resizeFinished(fence.fenceId)
    }
    ResizeHandle {
        anchors.right: parent.right; anchors.top: parent.top
        width: handleSize * 2; height: handleSize * 2; edges: 6; visible: !fence.rolledUp
        onResized: function(dx, dy, dw, dh) { applyResize(dx, dy, dw, dh) }
        onResizeFinished: fence.resizeFinished(fence.fenceId)
    }
    ResizeHandle {
        anchors.left: parent.left; anchors.bottom: parent.bottom
        width: handleSize * 2; height: handleSize * 2; edges: 9; visible: !fence.rolledUp
        onResized: function(dx, dy, dw, dh) { applyResize(dx, dy, dw, dh) }
        onResizeFinished: fence.resizeFinished(fence.fenceId)
    }
    ResizeHandle {
        anchors.right: parent.right; anchors.bottom: parent.bottom
        width: handleSize * 2; height: handleSize * 2; edges: 10; visible: !fence.rolledUp
        onResized: function(dx, dy, dw, dh) { applyResize(dx, dy, dw, dh) }
        onResizeFinished: fence.resizeFinished(fence.fenceId)
    }

    function applyResize(dx, dy, dw, dh) {
        var newW = Math.max(fence.width + dw, minWidth);
        var newH = Math.max(fence.height + dh, minHeight);
        var newX = fence.x + (newW !== fence.width + dw && dw < 0 ? 0 : dx);
        var newY = fence.y + (newH !== fence.height + dh && dh < 0 ? 0 : dy);

        fence.x = newX;
        fence.y = newY;
        fence.width = newW;
        fence.height = newH;
        fence.expandedHeight = newH;
        fence.sizeEdited(fence.fenceId, newX, newY, newW, newH);
    }

    function triggerRename() {
        titleBar.beginEdit();
    }
}
