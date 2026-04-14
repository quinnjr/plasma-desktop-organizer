import QtQuick
import QtQuick.Controls

Item {
    id: titleBar

    property string fenceId: ""
    property string title: ""
    property bool rolledUp: false

    signal dragMoved(real deltaX, real deltaY)
    signal dragEnded()
    signal rollupClicked()
    signal closeClicked()
    signal titleEdited(string newTitle)

    height: 28

    Rectangle {
        anchors.fill: parent
        color: "#cc1a1a2e"
        topLeftRadius: 6
        topRightRadius: 6
        bottomLeftRadius: titleBar.rolledUp ? 6 : 0
        bottomRightRadius: titleBar.rolledUp ? 6 : 0
    }

    // Drag area (whole title bar except buttons)
    MouseArea {
        id: dragArea
        anchors.left: parent.left
        anchors.right: buttonRow.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        cursorShape: Qt.SizeAllCursor

        property point pressPos

        onPressed: function(mouse) {
            pressPos = Qt.point(mouse.x, mouse.y);
        }
        onPositionChanged: function(mouse) {
            if (pressed) {
                titleBar.dragMoved(mouse.x - pressPos.x, mouse.y - pressPos.y);
            }
        }
        onReleased: titleBar.dragEnded()
        onDoubleClicked: titleLabel.startEditing()
    }

    // Title label (shown when not editing)
    Text {
        id: titleLabel
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: buttonRow.left
        text: titleBar.title
        color: "white"
        font.bold: true
        elide: Text.ElideRight
        visible: !nameEdit.visible

        function startEditing() {
            nameEdit.text = titleBar.title;
            nameEdit.visible = true;
            nameEdit.forceActiveFocus();
            nameEdit.selectAll();
        }
    }

    // Inline rename field
    TextField {
        id: nameEdit
        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.right: buttonRow.left
        anchors.verticalCenter: parent.verticalCenter
        visible: false
        background: Rectangle { color: "#33ffffff"; radius: 3 }
        color: "white"

        onAccepted: commit()
        Keys.onEscapePressed: visible = false

        function commit() {
            var t = text.trim();
            if (visible && t.length > 0) {
                titleBar.titleEdited(t);
            }
            visible = false;
        }

        Component.onDestruction: { if (visible) commit() }
    }

    // Rollup + close buttons
    Row {
        id: buttonRow
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        spacing: 2

        ToolButton {
            width: 20; height: 20
            text: titleBar.rolledUp ? "▼" : "▲"
            font.pixelSize: 10
            onClicked: titleBar.rollupClicked()
        }
        ToolButton {
            width: 20; height: 20
            text: "✕"
            font.pixelSize: 10
            onClicked: titleBar.closeClicked()
        }
    }
}
