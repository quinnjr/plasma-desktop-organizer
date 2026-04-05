import QtQuick

// Invisible drag handle for fence resizing.
// The 'edges' property is a bitfield: 1=left, 2=right, 4=top, 8=bottom.
// Combine for corners: 5=top-left, 6=top-right, 9=bottom-left, 10=bottom-right.

MouseArea {
    id: handle

    required property int edges

    signal resized(real dx, real dy, real dw, real dh)
    signal resizeFinished()

    property point _lastPos

    cursorShape: {
        var isLeft = edges & 1;
        var isRight = edges & 2;
        var isTop = edges & 4;
        var isBottom = edges & 8;
        if ((isLeft && isTop) || (isRight && isBottom))
            return Qt.SizeFDiagCursor;
        if ((isRight && isTop) || (isLeft && isBottom))
            return Qt.SizeBDiagCursor;
        if (isLeft || isRight)
            return Qt.SizeHorCursor;
        if (isTop || isBottom)
            return Qt.SizeVerCursor;
        return Qt.ArrowCursor;
    }

    onPressed: function(mouse) {
        _lastPos = Qt.point(mouse.x, mouse.y);
    }

    onPositionChanged: function(mouse) {
        if (!pressed) return;
        var deltaX = mouse.x - _lastPos.x;
        var deltaY = mouse.y - _lastPos.y;

        var dx = 0, dy = 0, dw = 0, dh = 0;
        if (edges & 1) { dx = deltaX; dw = -deltaX; }   // left
        if (edges & 2) { dw = deltaX; }                   // right
        if (edges & 4) { dy = deltaY; dh = -deltaY; }    // top
        if (edges & 8) { dh = deltaY; }                   // bottom

        handle.resized(dx, dy, dw, dh);
    }

    onReleased: handle.resizeFinished()
}
