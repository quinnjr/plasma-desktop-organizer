import QtQuick
import org.kde.kirigami as Kirigami

Rectangle {
    id: selRect

    property point startPoint: Qt.point(0, 0)
    property point endPoint: Qt.point(0, 0)

    visible: false
    color: Qt.rgba(Kirigami.Theme.highlightColor.r,
                   Kirigami.Theme.highlightColor.g,
                   Kirigami.Theme.highlightColor.b, 0.2)
    border.color: Kirigami.Theme.highlightColor
    border.width: 1

    x: Math.min(startPoint.x, endPoint.x)
    y: Math.min(startPoint.y, endPoint.y)
    width: Math.abs(endPoint.x - startPoint.x)
    height: Math.abs(endPoint.y - startPoint.y)

    function begin(point) {
        startPoint = point;
        endPoint = point;
        visible = true;
    }

    function update(point) {
        endPoint = point;
    }

    function finish() {
        visible = false;
        var rect = Qt.rect(x, y, width, height);
        startPoint = Qt.point(0, 0);
        endPoint = Qt.point(0, 0);
        return rect;
    }

    function cancel() {
        visible = false;
        startPoint = Qt.point(0, 0);
        endPoint = Qt.point(0, 0);
    }
}
