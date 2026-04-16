import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM
import org.kde.plasma.organizer.kcm 1.0

KCM.ScrollViewKCM {
    id: root

    implicitWidth: Kirigami.Units.gridUnit * 40
    implicitHeight: Kirigami.Units.gridUnit * 30

    property int currentPageIndex: 0

    SettingsModel {
        id: settingsModel
    }

    // Status bar at top
    header: RowLayout {
        spacing: Kirigami.Units.largeSpacing

        QQC2.Label {
            text: DBusClient.isRunning ? "Status: Running" : "Status: Stopped"
        }

        Rectangle {
            width: Kirigami.Units.gridUnit * 0.5
            height: Kirigami.Units.gridUnit * 0.5
            radius: width / 2
            color: DBusClient.isRunning ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.neutralTextColor
        }

        Item { Layout.fillWidth: true }

        QQC2.Button {
            text: "Stop"
            icon.name: "process-stop"
            enabled: DBusClient.isRunning
            onClicked: DBusClient.stop()
        }

        QQC2.Button {
            text: "Restart"
            icon.name: "system-reboot"
            enabled: DBusClient.isRunning
            onClicked: DBusClient.restart()
        }
    }

    // Sidebar with page list
    Kirigami.OverlayDrawer {
        id: drawer
        edge: Qt.LeftEdge
        modal: false
        width: Kirigami.Units.gridUnit * 10

        Kirigami.Theme.colorSet: Kirigami.Theme.View

        contentItem: ColumnLayout {
            spacing: 0

            QQC2.ItemDelegate {
                text: "General"
                Layout.fillWidth: true
                highlighted: root.currentPageIndex === 0
                onClicked: {
                    stackView.replace(generalPage)
                    root.currentPageIndex = 0
                }
            }

            QQC2.ItemDelegate {
                text: "Appearance"
                Layout.fillWidth: true
                highlighted: root.currentPageIndex === 1
                onClicked: {
                    stackView.replace(appearancePage)
                    root.currentPageIndex = 1
                }
            }

            QQC2.ItemDelegate {
                text: "Advanced"
                Layout.fillWidth: true
                highlighted: root.currentPageIndex === 2
                onClicked: {
                    stackView.replace(advancedPage)
                    root.currentPageIndex = 2
                }
            }

            QQC2.ItemDelegate {
                text: "About"
                Layout.fillWidth: true
                highlighted: root.currentPageIndex === 3
                onClicked: {
                    stackView.replace(aboutPage)
                    root.currentPageIndex = 3
                }
            }

            Item { Layout.fillHeight: true }
        }
    }

    // Main content area
    view: QQC2.StackView {
        id: stackView
        initialItem: generalPage

        Component {
            id: generalPage
            GeneralPage {
                model: settingsModel
            }
        }

        Component {
            id: appearancePage
            AppearancePage {
                model: settingsModel
            }
        }

        Component {
            id: advancedPage
            AdvancedPage {
                model: settingsModel
            }
        }

        Component {
            id: aboutPage
            AboutPage {}
        }
    }

    // Footer with Apply/Reset/Defaults buttons
    footer: RowLayout {
        QQC2.Button {
            text: "Apply"
            icon.name: "dialog-ok-apply"
            enabled: settingsModel.needsSave
            onClicked: {
                settingsModel.save();
                DBusClient.reloadConfig();
            }
        }

        QQC2.Button {
            text: "Reset"
            icon.name: "edit-undo"
            enabled: settingsModel.needsSave
            onClicked: settingsModel.load()
        }

        QQC2.Button {
            text: "Defaults"
            icon.name: "edit-clear"
            onClicked: settingsModel.defaults()
        }
    }
}
