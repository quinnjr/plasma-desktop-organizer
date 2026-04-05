import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents

Item {
    id: titleBar

    required property string title
    required property bool rolledUp

    signal titleChanged(string newTitle)
    signal rollupToggled()
    signal closeClicked()
    signal dragBy(real deltaX, real deltaY)
    signal dragFinished()

    height: 28

    function beginEdit() {
        titleEditor.visible = true;
        titleEditor.text = title;
        titleEditor.selectAll();
        titleEditor.forceActiveFocus();
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 4
        spacing: 4

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            PlasmaComponents.Label {
                id: titleLabel
                anchors.fill: parent
                anchors.rightMargin: 4
                verticalAlignment: Text.AlignVCenter
                text: titleBar.title
                elide: Text.ElideRight
                font.weight: Font.Medium
                visible: !titleEditor.visible
            }

            PlasmaComponents.TextField {
                id: titleEditor
                anchors.fill: parent
                visible: false
                font.weight: Font.Medium

                onAccepted: {
                    if (text.trim() !== "") {
                        titleBar.titleChanged(text.trim());
                    }
                    visible = false;
                }
                onActiveFocusChanged: {
                    if (!activeFocus && visible) {
                        visible = false;
                    }
                }
            }

            MouseArea {
                id: dragArea
                anchors.fill: parent
                cursorShape: Qt.OpenHandCursor
                visible: !titleEditor.visible

                property point lastPos

                onPressed: function(mouse) {
                    lastPos = Qt.point(mouse.x, mouse.y);
                    cursorShape = Qt.ClosedHandCursor;
                }

                onPositionChanged: function(mouse) {
                    if (pressed) {
                        titleBar.dragBy(mouse.x - lastPos.x, mouse.y - lastPos.y);
                    }
                }

                onReleased: {
                    cursorShape = Qt.OpenHandCursor;
                    titleBar.dragFinished();
                }

                onDoubleClicked: {
                    titleBar.beginEdit();
                }
            }
        }

        PlasmaComponents.ToolButton {
            Layout.preferredWidth: 22
            Layout.preferredHeight: 22
            icon.name: titleBar.rolledUp ? "arrow-down" : "arrow-up"
            icon.width: 14
            icon.height: 14
            onClicked: titleBar.rollupToggled()

            PlasmaComponents.ToolTip {
                text: titleBar.rolledUp ? "Expand" : "Roll up"
            }
        }

        PlasmaComponents.ToolButton {
            Layout.preferredWidth: 22
            Layout.preferredHeight: 22
            icon.name: "window-close"
            icon.width: 14
            icon.height: 14
            onClicked: titleBar.closeClicked()

            PlasmaComponents.ToolTip {
                text: "Delete fence"
            }
        }
    }
}
