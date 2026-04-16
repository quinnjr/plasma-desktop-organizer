import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.FormLayout {
    id: root

    property var model

    // Fence Style section
    Item {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: "Fence Style"
    }

    RowLayout {
        Kirigami.FormData.label: "Background opacity:"

        QQC2.Slider {
            id: opacitySlider
            from: 0
            to: 100
            stepSize: 5
            value: model.fenceOpacity
            onMoved: model.fenceOpacity = value
            Layout.fillWidth: true
        }

        QQC2.Label {
            text: opacitySlider.value + "%"
            Layout.minimumWidth: Kirigami.Units.gridUnit * 3
        }
    }

    QQC2.TextField {
        Kirigami.FormData.label: "Background color:"
        text: model.fenceBackgroundColor
        onTextChanged: model.fenceBackgroundColor = text
        placeholderText: "#2c2c2c"
    }

    RowLayout {
        Kirigami.FormData.label: "Title bar height:"

        QQC2.SpinBox {
            from: 24
            to: 48
            value: model.titleBarHeight
            onValueModified: model.titleBarHeight = value
        }

        QQC2.Label {
            text: "pixels"
        }
    }

    // Behavior section
    Item {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: "Behavior"
    }

    QQC2.CheckBox {
        Kirigami.FormData.label: "Grid snapping:"
        text: "Snap to grid"
        checked: model.snapToGrid
        onToggled: model.snapToGrid = checked
    }

    RowLayout {
        Kirigami.FormData.label: "Grid size:"
        enabled: model.snapToGrid

        QQC2.SpinBox {
            from: 10
            to: 50
            value: model.gridSize
            onValueModified: model.gridSize = value
        }

        QQC2.Label {
            text: "pixels"
        }
    }

    QQC2.CheckBox {
        Kirigami.FormData.label: "Effects:"
        text: "Enable animations"
        checked: model.enableAnimations
        onToggled: model.enableAnimations = checked
    }
}
