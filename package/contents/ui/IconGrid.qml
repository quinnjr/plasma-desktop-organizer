import QtQuick
import org.kde.plasma.components as PlasmaComponents

Item {
    id: gridRoot

    required property var icons
    required property int iconSize

    property int spacing: 16
    property int columns: Math.max(1, Math.floor(width / cellWidth))
    property int cellWidth: iconSize + spacing + 16
    property int cellHeight: iconSize + 36 + spacing
    property string selectedUrl: ""

    signal iconClicked(string url, var mouse)
    signal iconDoubleClicked(string url)
    signal iconDragStarted(string url)
    signal iconDragMoved(string url, real sceneX, real sceneY)
    signal iconDragEnded(string url, real sceneX, real sceneY)

    clip: true

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: width
        contentHeight: {
            var rows = Math.ceil(repeater.count / Math.max(1, gridRoot.columns));
            return Math.max(height, rows * gridRoot.cellHeight);
        }
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds

        PlasmaComponents.ScrollBar.vertical: PlasmaComponents.ScrollBar {
            policy: flickable.contentHeight > flickable.height ?
                PlasmaComponents.ScrollBar.AsNeeded : PlasmaComponents.ScrollBar.AlwaysOff
        }

        Item {
            width: flickable.width
            height: flickable.contentHeight

            Repeater {
                id: repeater
                model: gridRoot.icons

                DesktopIcon {
                    required property var modelData
                    required property int index

                    fileUrl: modelData.url
                    fileName: modelData.fileName
                    fileIsDir: modelData.isDir || false
                    iconSize: gridRoot.iconSize
                    selected: fileUrl === gridRoot.selectedUrl

                    x: modelData.col * gridRoot.cellWidth
                    y: modelData.row * gridRoot.cellHeight

                    onClicked: function(mouse) {
                        gridRoot.iconClicked(fileUrl, mouse);
                    }
                    onDoubleClicked: {
                        gridRoot.iconDoubleClicked(fileUrl);
                    }
                    onDragStarted: {
                        gridRoot.iconDragStarted(fileUrl);
                    }
                    onDragMoved: function(sx, sy) {
                        gridRoot.iconDragMoved(fileUrl, sx, sy);
                    }
                    onDragEnded: function(sx, sy) {
                        gridRoot.iconDragEnded(fileUrl, sx, sy);
                    }
                }
            }
        }
    }
}
