import QtQuick 2.0
import Sailfish.Silica 1.0

import FileListModel 1.0
import Controller 1.0

import "../items"

CoverBackground {
    id: cover

    property int download_id: 0
    property int current_index: 0

    FileListModel {
        id: fileListModel
        folder_id: download_id
    }

    Image {
        fillMode: Image.PreserveAspectFit
        height: 172
        source: "qrc:///icons/172x172/harbour-was.png"
        anchors.centerIn: parent
        opacity: 0.2
    }

    BusyIndicator {
        id: loadingIndicator
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        running: cover.status === Cover.Active
        size: BusyIndicatorSize.Small
        visible: false

        anchors.margins: Theme.horizontalPageMargin
    }

    SilicaListView {
        id: listView

        model: fileListModel

        anchors.fill: parent

        highlightFollowsCurrentItem: false

        currentIndex: current_index
        snapMode: ListView.SnapOneItem
        highlightRangeMode: ListView.StrictlyEnforceRange
        cacheBuffer: 0

        highlight: highlight

        delegate: Item {
            width: cover.width
            height: cover.height + 1
            x: 0

            AnimatedImage {
                parent_width: parent.width
                parent_height: parent.height

                picture_source: model.photo_source

                onLoadingPictureChanged: {
                    loadingIndicator.visible = loadingPicture === Image.Loading

                    if (loadingPicture === Image.Ready) {
                        animation_show.start()
                    }
                }
            }
        }

        Component {
            id: highlight
            Rectangle {
                width: 0
                height: 0
                x: 0
                y: listView.currentItem.y
            }
        }

        SequentialAnimation on opacity {
            id: animation_hide
            running: cover.status === Cover.Active
            loops: Animation.Infinite

            PauseAnimation {
                duration: 25000
            }

            NumberAnimation {
                duration: 1000
                from: 1.0
                to: 0
                easing.type: Easing.InQuad
            }

            ScriptAction {
                script: {
                    var index = Math.floor(Math.random() * listView.count)
                    if (index !== current_index) {
                        current_index = index
                    } else {
                        animation_show.start()
                    }

                }
            }
        }

        SequentialAnimation on opacity {
            id: animation_show
            running: cover.status === Cover.Active

            NumberAnimation {
                duration: 1000
                from: 0
                to: 1.0
                easing.type: Easing.InQuad
            }
        }
    }
}
