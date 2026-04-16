import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

ColumnLayout {
    id: root
    spacing: Kirigami.Units.largeSpacing

    Kirigami.Icon {
        source: "view-split-left-right"
        Layout.preferredWidth: Kirigami.Units.iconSizes.huge
        Layout.preferredHeight: Kirigami.Units.iconSizes.huge
        Layout.alignment: Qt.AlignHCenter
    }

    Kirigami.Heading {
        text: "Plasma Desktop Organizer"
        level: 1
        Layout.alignment: Qt.AlignHCenter
    }

    QQC2.Label {
        text: "Version " + (DBusClient.version || "0.1.0")
        Layout.alignment: Qt.AlignHCenter
    }

    QQC2.Label {
        text: "A Fences-style desktop organizer for KDE Plasma"
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Item { Layout.preferredHeight: Kirigami.Units.largeSpacing }

    QQC2.Label {
        text: "License: MIT"
        Layout.alignment: Qt.AlignHCenter
    }

    QQC2.Label {
        text: "© 2026 Joseph R. Quinn"
        Layout.alignment: Qt.AlignHCenter
    }

    QQC2.Label {
        text: '<a href="https://github.com/quinnjr/plasma-desktop-organizer">GitHub Repository</a>'
        onLinkActivated: Qt.openUrlExternally(link)
        Layout.alignment: Qt.AlignHCenter
    }

    Item { Layout.fillHeight: true }
}
