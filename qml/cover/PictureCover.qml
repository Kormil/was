import QtQuick 2.0
import Sailfish.Silica 1.0

import "../items"

CoverBackground {
    property string picture_source: ""

    id: cover

    SilicaFlickable {
        anchors.fill: cover

        AnimatedImage {
            parent_width: cover.width
            parent_height: cover.height
            x: 0
            picture_source: cover.picture_source
        }
    }
}
