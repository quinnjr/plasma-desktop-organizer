import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Item {
    id: configPage

    property int cfg_iconSize
    property int cfg_fenceOpacity
    property int cfg_animationSpeed
    property bool cfg_doubleClickToHide
    property string cfg_defaultFenceTitle

    Kirigami.FormLayout {
        anchors.left: parent.left
        anchors.right: parent.right

        QQC2.ComboBox {
            id: iconSizeCombo
            Kirigami.FormData.label: i18n("Default icon size:")
            model: [
                { text: i18n("Small (32px)"), value: 32 },
                { text: i18n("Medium (48px)"), value: 48 },
                { text: i18n("Large (64px)"), value: 64 },
                { text: i18n("Extra Large (80px)"), value: 80 }
            ]
            textRole: "text"
            valueRole: "value"

            Component.onCompleted: currentIndex = indexOfValue(cfg_iconSize)
            onActivated: cfg_iconSize = currentValue
        }

        QQC2.Slider {
            id: opacitySlider
            Kirigami.FormData.label: i18n("Fence opacity: %1%", Math.round(value))
            from: 20
            to: 100
            stepSize: 5
            value: cfg_fenceOpacity
            onMoved: cfg_fenceOpacity = Math.round(value)
        }

        QQC2.Slider {
            id: animationSlider
            Kirigami.FormData.label: {
                var ms = Math.round(value);
                if (ms === 0) return i18n("Animation speed: Instant");
                return i18n("Animation speed: %1 ms", ms);
            }
            from: 0
            to: 500
            stepSize: 50
            value: cfg_animationSpeed
            onMoved: cfg_animationSpeed = Math.round(value)
        }

        QQC2.CheckBox {
            id: doubleClickCheck
            Kirigami.FormData.label: i18n("Desktop gestures:")
            text: i18n("Double-click to hide/show all fences")
            checked: cfg_doubleClickToHide
            onToggled: cfg_doubleClickToHide = checked
        }

        QQC2.TextField {
            id: titleField
            Kirigami.FormData.label: i18n("Default fence title:")
            text: cfg_defaultFenceTitle
            onTextEdited: cfg_defaultFenceTitle = text
        }
    }
}
