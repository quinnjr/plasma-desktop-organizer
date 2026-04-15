import QtQuick
import QtQuick.Controls

Item {
    id: gridRoot

    property var fileModel: null
    property int iconSize: 48
    property string selectedUrl: ""

    signal iconClicked(string url, var mouse)
    signal iconDoubleClicked(string url)
    signal iconDragStarted(string url)
    signal iconDragMoved(string url, real sceneX, real sceneY)
    signal iconDragEnded(string url, real sceneX, real sceneY)

    readonly property int columnWidth: iconSize + 24
    readonly property int columnCount: Math.max(1, Math.floor(width / columnWidth))

    Flickable {
        anchors.fill: parent
        contentHeight: grid.implicitHeight
        clip: true

        Grid {
            id: grid
            anchors.left: parent.left
            anchors.right: parent.right
            columns: gridRoot.columnCount
            spacing: 4
            padding: 8

            Repeater {
                model: gridRoot.fileModel

                delegate: DesktopIcon {
                    required property var model

                    fileUrl: model.fileUrl
                    fileName: model.fileName
                    iconName: model.iconName
                    displayName: model.displayName
                    iconSize: gridRoot.iconSize
                    selected: model.fileUrl === gridRoot.selectedUrl

                    onClicked: function(mouse) {
                        gridRoot.iconClicked(model.fileUrl, mouse);
                    }
                    onDoubleClicked: function(mouse) {
                        gridRoot.iconDoubleClicked(model.fileUrl);
                    }
                    onDragStarted: function(url) {
                        gridRoot.iconDragStarted(url);
                    }
                    onDragMoved: function(url, sx, sy) {
                        gridRoot.iconDragMoved(url, sx, sy);
                    }
                    onDragEnded: function(url, sx, sy) {
                        gridRoot.iconDragEnded(url, sx, sy);
                    }
                }
            }
        }
    }
}
