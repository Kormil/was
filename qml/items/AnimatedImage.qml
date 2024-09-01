import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    property int parent_width: 0
    property int parent_height: 0
    property string picture_source: ""
    property bool play_anim_x: false
    property bool play_anim_y: false

    property int loadingPicture: image.status

    width: parent_width
    height: parent_height

    SequentialAnimation on x {
        id: animation_on_x
        running: cover.status === Cover.Active && play_anim_x
        loops: Animation.Infinite

        PauseAnimation { duration: 5 * 1000 }

        NumberAnimation {
            duration: 20000
            to: -(image.width - parent_width)
            easing.type: Easing.InQuad
        }

        PauseAnimation { duration: 30 * 1000 }

        NumberAnimation {
            duration: 20000
            to: 0
            easing.type: Easing.InQuad
        }

        PauseAnimation { duration: 30 * 1000 }
    }

    SequentialAnimation on y {
        id: animation_on_y
        running: cover.status === Cover.Active && play_anim_y
        loops: Animation.Infinite

        PauseAnimation { duration: 5 * 1000 }

        NumberAnimation {
            duration: 20000
            to: -(image.height - parent_height)
            easing.type: Easing.InQuad
        }

        PauseAnimation { duration: 30 * 1000 }

        NumberAnimation {
            duration: 20000
            to: 0
            easing.type: Easing.InQuad
        }

        PauseAnimation { duration: 30 * 1000 }
    }

    Image {
        property double calculated_width

        id: image

        anchors {
            top: parent.top
            left: parent.left
        }

        smooth: false
        width: parent_height
        height: parent_height

        source: picture_source

        onStatusChanged: {
            if (image.status === Image.Ready) {
                calculated_width = (parent_height / sourceSize.height) * sourceSize.width

                if (calculated_width >= parent_width) {
                    width = calculated_width
                    height = parent_height
                    play_anim_y = false
                    play_anim_x = true
                } else {
                    width = parent_width
                    height = (parent_width / sourceSize.width) * sourceSize.height
                    play_anim_y = true
                    play_anim_x = false
                }
            }
        }
    }
}
