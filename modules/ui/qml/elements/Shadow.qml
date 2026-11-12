// Shadow effect

import QtQuick
import QtQuick.Effects
import Revise as Revise

MultiEffect {
    visible: !Revise.BuildInfo.isMobileBuild
    shadowEnabled: !Revise.BuildInfo.isMobileBuild
    shadowHorizontalOffset: 4
    shadowVerticalOffset: 4
    shadowBlur: 0.4
    shadowColor: Qt.color("#20000000")
}
