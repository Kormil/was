import QtQuick 2.0
import Sailfish.Silica 1.0

ListItem {
    contentHeight: Theme.itemSizeMedium

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightBackgroundColor, 0.10) }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }

    Image {
        id: icon

        height: parent.height - Theme.horizontalPageMargin
        width: height

        x: Theme.horizontalPageMargin

        anchors {
            verticalCenter: parent.verticalCenter
        }

        fillMode: Image.PreserveAspectFit
        source: model.is_dir ? "image://theme/icon-m-file-folder" : "image://photos/" + model.cache_key
        smooth: false
        cache: true
    }

    Label {
        id: fileLabel
        text: model.name
        height: parent.height / 2.0
        truncationMode: TruncationMode.Fade

        anchors {
            left: icon.right
            verticalCenter: parent.verticalCenter
            leftMargin: Theme.horizontalPageMargin
        }

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Label {
        id: timeLabel
        text: model.create_time
        height: Theme.itemSizeExtraSmall
        font.pixelSize: Theme.fontSizeExtraSmall

        anchors {
            left: fileLabel.left
            bottom: parent.bottom
        }

        horizontalAlignment: Text.AlignBottom
        verticalAlignment: Text.AlignBottom
    }

    onClicked: {
        if (model.is_dir === true) {
            pageStack.animatorPush(Qt.resolvedUrl("../pages/FirstPage.qml"), {download_id: model.id, dir_name: model.name})
        } else {
            pageStack.animatorPush(Qt.resolvedUrl("../pages/PicturePage.qml"), {fileImage: model})
        }
    }
}
