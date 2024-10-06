import QtQuick 2.0
import Sailfish.Silica 1.0

import Settings 1.0
import FolderListModel 1.0
import Controller 1.0

import "../items"

Page {
    property bool ready: false
    property int download_id: 0
    property string dir_name: qsTr("Photos")

    id: page

    onActiveFocusChanged: {
        if (activeFocus) {
            resetCoverPage()
            changeFolderForCover(download_id)
        }
    }

    FolderListModel {
        id: folderListModel
        folder_id: download_id
    }

    Connections {
        target: folderListModel
        onFilesLoading: {
            if (!ready) {
                loadingIndicator.running = true
                loadingIndicator.visible = true
                filesView.visible = false
            }
        }
    }

    Connections {
        target: folderListModel
        onFoldersLoaded: {
            if (ready) {
                loadingIndicator.running = false
                loadingIndicator.visible = false
                filesView.visible = true
            } else {
                ready = true
            }
        }
    }

    Connections {
        target: folderListModel
        onFilesLoaded: {
            if (ready) {
                loadingIndicator.running = false
                loadingIndicator.visible = false
                filesView.visible = true
            } else {
                ready = true
            }
        }
    }

    Component.onCompleted: {
        Controller.getFolderSize(download_id)
    }

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        anchors.fill: parent

            SilicaListView {
                id: filesView

                model: folderListModel

                anchors.fill: parent
                spacing: 0

                header: PageHeader {
                    id: title
                    title: dir_name
                }

                delegate: DictionaryPictureItem {
                    onClicked: {
                        if (model.is_dir) {
                            changeFolderForCover(model.id)
                            pageStack.push(Qt.resolvedUrl("../pages/SecondPage.qml"), {download_id: model.id, dir_name: model.name})
                        } else {
                            pageStack.push(Qt.resolvedUrl("../pages/PicturePage.qml"), {download_id: download_id, started_index: folderListModel.mapToFileListModel(index)})
                        }
                    }
                }

                VerticalScrollDecorator {}
            }

        BusyIndicator {
            id: loadingIndicator
            anchors.centerIn: parent
            running: true
            size: BusyIndicatorSize.Large
            visible: false
        }
    }
}
