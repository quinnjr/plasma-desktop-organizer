import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents

import "lib/IconHelper.js" as IconHelper

Item {
    id: iconRoot

    required property string fileUrl
    required property string fileName
    required property bool fileIsDir
    required property int iconSize

    property bool selected: false
    property string iconName: IconHelper.iconForFile(fileName, fileIsDir)
    property string displayName: fileName

    signal clicked(var mouse)
    signal doubleClicked(var mouse)
    signal dragStarted()
    signal dragMoved(real sceneX, real sceneY)
    signal dragEnded(real sceneX, real sceneY)

    width: iconSize + 16
    height: iconSize + labelText.implicitHeight + 12

    Component.onCompleted: {
        if (fileName.endsWith(".desktop")) {
            IconHelper.loadDesktopFileInfo(fileUrl, function(info) {
                if (info.icon) iconName = info.icon;
                if (info.name) displayName = info.name;
            });
        }
    }

    Rectangle {
        anchors.fill: parent
        color: Kirigami.Theme.highlightColor
        opacity: 0.3
        radius: 4
        visible: iconRoot.selected
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 2

        Kirigami.Icon {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: iconRoot.iconSize
            Layout.preferredHeight: iconRoot.iconSize
            source: iconRoot.iconName
        }

        PlasmaComponents.Label {
            id: labelText
            Layout.fillWidth: true
            text: iconRoot.displayName
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideMiddle
            maximumLineCount: 2
            wrapMode: Text.Wrap
            font.pointSize: Kirigami.Theme.smallFont.pointSize
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
            console.log("ICON PRESSED:", iconRoot.fileName, "at", mouse.x, mouse.y, "size:", iconRoot.width, iconRoot.height);
            pressPos = Qt.point(mouse.x, mouse.y);
            wasDragged = false;
        }

        onPositionChanged: function(mouse) {
            if (!wasDragged && pressed) {
                var dx = mouse.x - pressPos.x;
                var dy = mouse.y - pressPos.y;
                if (Math.sqrt(dx * dx + dy * dy) > 8) {
                    wasDragged = true;
                    iconRoot.dragStarted();
                }
            }
            if (wasDragged && pressed) {
                var sp = mouseArea.mapToItem(null, mouse.x, mouse.y);
                iconRoot.dragMoved(sp.x, sp.y);
            }
        }

        onReleased: function(mouse) {
            if (wasDragged) {
                var sp = mouseArea.mapToItem(null, mouse.x, mouse.y);
                iconRoot.dragEnded(sp.x, sp.y);
            }
        }

        onClicked: function(mouse) {
            if (!wasDragged) {
                iconRoot.clicked(mouse);
            }
        }

        onDoubleClicked: function(mouse) {
            iconRoot.doubleClicked(mouse);
        }
    }
}
