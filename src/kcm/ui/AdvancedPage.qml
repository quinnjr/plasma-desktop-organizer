import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.FormLayout {
    id: root

    property var model

    // File Storage section
    Item {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: "File Storage"
    }

    ColumnLayout {
        Kirigami.FormData.label: "Data directory:"

        QQC2.TextField {
            Layout.fillWidth: true
            text: model.dataDirectory
            onTextChanged: model.dataDirectory = text
        }

        QQC2.Label {
            text: "(Requires restart to take effect)"
            font.italic: true
            font.pointSize: Kirigami.Theme.smallFont.pointSize
        }
    }

    // Size Limits section
    Item {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: "Size Limits"
    }

    RowLayout {
        Kirigami.FormData.label: "Minimum fence width:"

        QQC2.SpinBox {
            from: 50
            to: 200
            value: model.minFenceWidth
            onValueModified: model.minFenceWidth = value
        }

        QQC2.Label {
            text: "pixels"
        }
    }

    RowLayout {
        Kirigami.FormData.label: "Minimum fence height:"

        QQC2.SpinBox {
            from: 50
            to: 200
            value: model.minFenceHeight
            onValueModified: model.minFenceHeight = value
        }

        QQC2.Label {
            text: "pixels"
        }
    }

    // Click Behavior section
    Item {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: "Click Behavior"
    }

    RowLayout {
        Kirigami.FormData.label: "Click-through delay:"

        QQC2.SpinBox {
            from: 0
            to: 500
            stepSize: 50
            value: model.clickThroughDelay
            onValueModified: model.clickThroughDelay = value
        }

        QQC2.Label {
            text: "ms"
        }
    }
}
