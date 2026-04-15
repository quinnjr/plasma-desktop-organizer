import QtQuick
import QtQuick.Controls

Item {
    id: root
    anchors.fill: parent

    property bool fencesVisible: true
    property string selectedUrl: ""

    Repeater {
        id: fenceRepeater
        model: fenceModel

        delegate: FenceContainer {
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
            height: rolledUp ? 28 : fenceHeight

            fenceId: fenceId
            title: title
            rolledUp: rolledUp
            iconSize: iconSize

            opacity: root.fencesVisible ? 1.0 : 0.0
            Behavior on opacity {
                NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
            }

            onTitleEdited: function(fid, newTitle) {
                fenceManager.renameFence(fid, newTitle);
            }
            onRollupToggled: function(fid, newRolledUp) {
                fenceManager.setRolledUp(fid, newRolledUp);
            }
            onCloseRequested: function(fid) {
                fenceManager.deleteFence(fid);
            }
            onMoveFinished: function(fid, nx, ny) {
                fenceManager.moveFence(fid, nx, ny);
            }
            onResizeFinished: function(fid, nx, ny, nw, nh) {
                fenceManager.resizeFence(fid, nx, ny, nw, nh);
            }

            selectedUrl: root.selectedUrl

            onIconClicked: function(fid, url, mouse) {
                root.selectedUrl = url;
            }
            onIconDoubleClicked: function(fid, url) {
                Qt.openUrlExternally(url);
            }
        }
    }
}
