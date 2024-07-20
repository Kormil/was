import QtQuick 2.0
import Sailfish.Silica 1.0

ListItem {
    contentHeight: Theme.itemSizeMedium

    Label {
        id: item_counter
        text: model.item_counter
        height: parent.height / 2.0
        truncationMode: TruncationMode.Fade
        visible: model.is_dir

        anchors {
            right: icon.left
            verticalCenter: parent.verticalCenter
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
        }

        horizontalAlignment: Text.AlignHCenters
        verticalAlignment: Text.AlignVCenter
        color: Theme.secondaryColor
    }

    Item {
        id: icon

        height: parent.height
        width: height

        x: Theme.horizontalPageMargin + width

        Image {

            height: parent.height
            width: parent.width

            anchors {
                verticalCenter: parent.verticalCenter
            }

            fillMode: Image.PreserveAspectFit
            source: model.cache_key
            smooth: false
            cache: true

            onStatusChanged: {
                if (status == Image.Ready) {
                    loadingImageIndicator.running = false
                    loadingImageIndicator.visible = false
                } else if (status == Image.Loading) {
                    loadingImageIndicator.running = true
                    loadingImageIndicator.visible = true
                }
            }
        }
        BusyIndicator {
            id: loadingImageIndicator
            anchors.centerIn: parent
            running: true
            size: BusyIndicatorSize.Small
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            visible: false
        }

    }

    Label {
        id: fileLabel
        text: model.name
        height: parent.height / 2.0
        truncationMode: TruncationMode.Fade
        color: Theme.primaryColor

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
        color: Theme.secondaryColor

        anchors {
            left: fileLabel.left
            bottom: parent.bottom
        }

        horizontalAlignment: Text.AlignBottom
        verticalAlignment: Text.AlignBottom
    }
}
