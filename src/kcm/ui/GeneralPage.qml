import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.FormLayout {
    id: root

    property var model

    // Startup section
    Item {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: "Startup"
    }

    QQC2.CheckBox {
        Kirigami.FormData.label: "Autostart:"
        text: "Start automatically on login"
        checked: model.autoStart
        onToggled: model.autoStart = checked
    }

    QQC2.CheckBox {
        Kirigami.FormData.label: "Initial state:"
        text: "Start minimized"
        checked: model.startMinimized
        onToggled: model.startMinimized = checked
    }

    // System Tray section
    Item {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: "System Tray"
    }

    QQC2.CheckBox {
        Kirigami.FormData.label: "Visibility:"
        text: "Show system tray icon"
        checked: model.showSystemTray
        onToggled: model.showSystemTray = checked
    }

    // Defaults section
    Item {
        Kirigami.FormData.isSection: true
        Kirigami.FormData.label: "Defaults"
    }

    RowLayout {
        Kirigami.FormData.label: "Default icon size:"

        QQC2.SpinBox {
            from: 32
            to: 96
            stepSize: 8
            value: model.defaultIconSize
            onValueModified: model.defaultIconSize = value
        }

        QQC2.Label {
            text: "pixels"
        }
    }
}
