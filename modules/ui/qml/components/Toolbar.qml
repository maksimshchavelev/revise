// Toolbar.qml

import QtQuick
import Revise as Revise

Loader {
    sourceComponent: Revise.SettingsService.useMobileUi ? mobileToolbar : desktopToolbar

    Component {
        id: desktopToolbar
        Revise.DesktopToolbar {}
    }

    Component {
        id: mobileToolbar
        Revise.MobileToolbar {}
    }
}
