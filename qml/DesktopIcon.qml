import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Item {
    id: iconRoot

    property string fileUrl: ""
    property string fileName: ""
    property string iconName: "application-x-executable"
    property string displayName: ""
    property int iconSize: 48
    property bool selected: false

    signal clicked(var mouse)
    signal doubleClicked(var mouse)
    signal dragStarted(string url)
    signal dragMoved(string url, real sceneX, real sceneY)
    signal dragEnded(string url, real sceneX, real sceneY)

    Drag.active: mouseArea.wasDragged && mouseArea.pressed
    Drag.dragType: Drag.Automatic
    Drag.supportedActions: Qt.MoveAction
    Drag.mimeData: { "text/uri-list": fileUrl + "\r\n" }
    Drag.hotSpot.x: width / 2
    Drag.hotSpot.y: height / 2

    width: iconSize + 16
    height: iconSize + labelText.implicitHeight + 12

    Rectangle {
        anchors.fill: parent
        color: "#44aaaaff"
        radius: 4
        visible: iconRoot.selected
    }

    Column {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 2

        Kirigami.Icon {
            anchors.horizontalCenter: parent.horizontalCenter
            width: iconRoot.iconSize
            height: iconRoot.iconSize
            source: iconRoot.iconName
        }

        Text {
            id: labelText
            width: parent.width
            text: iconRoot.displayName || iconRoot.fileName
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideMiddle
            maximumLineCount: 2
            wrapMode: Text.Wrap
            font.pixelSize: 11
            style: Text.Outline
            styleColor: "black"
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true

        property point pressPos
        property bool wasDragged: false

        onPressed: function(mouse) {
            pressPos = Qt.point(mouse.x, mouse.y);
            wasDragged = false;
        }
        onPositionChanged: function(mouse) {
            if (!wasDragged && pressed) {
                var dx = mouse.x - pressPos.x;
                var dy = mouse.y - pressPos.y;
                if (Math.sqrt(dx*dx + dy*dy) > 8) {
                    wasDragged = true;
                    iconRoot.dragStarted(iconRoot.fileUrl);
                }
            }
            if (wasDragged && pressed) {
                var sp = mouseArea.mapToItem(null, mouse.x, mouse.y);
                iconRoot.dragMoved(iconRoot.fileUrl, sp.x, sp.y);
            }
        }
        onReleased: function(mouse) {
            if (wasDragged) {
                var sp = mouseArea.mapToItem(null, mouse.x, mouse.y);
                iconRoot.dragEnded(iconRoot.fileUrl, sp.x, sp.y);
            }
        }
        onClicked: function(mouse) {
            if (!wasDragged) iconRoot.clicked(mouse);
        }
        onDoubleClicked: function(mouse) {
            iconRoot.doubleClicked(mouse);
        }
    }
}
