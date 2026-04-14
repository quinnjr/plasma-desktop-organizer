import QtQuick

Item {
    id: root
    anchors.fill: parent

    // Temporary: colored rectangle to confirm the overlay is visible
    Rectangle {
        width: 200
        height: 80
        color: "#4400aaff"
        radius: 8
        x: 50
        y: 50

        Text {
            anchors.centerIn: parent
            text: "plasma-organizer\n" + screenName
            color: "white"
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
