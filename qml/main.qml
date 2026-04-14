import QtQuick
import QtQuick.Controls

Item {
    id: root
    anchors.fill: parent

    Repeater {
        model: fenceModel

        delegate: Rectangle {
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
            height: rolledUp ? 32 : fenceHeight
            color: "#44000088"
            border.color: "#aa6666ff"
            border.width: 1
            radius: 6

            Text {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 8
                text: title
                color: "white"
                font.bold: true
            }
        }
    }
}
