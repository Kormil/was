import QtQuick 2.0
import Sailfish.Silica 1.0

import Settings 1.0
import FileListModel 1.0
import Controller 1.0

import "../items"

Page {
    property int download_id: 0;
    property string dir_name: qsTr("Photos")

    id: page

    onActiveFocusChanged: {
        resetCoverPage()
    }

    FileListModel {
        id: foldersModel
    }

    FileListModel {
        id: filesModel
    }

    Connections {
        target: Controller
        onFoldersLoading: {
            loadingIndicator.running = true
            loadingIndicator.visible = true
            foldersView.visible = false
            filesView.visible = false
        }
    }

    Connections {
        target: Controller
        onFoldersLoaded: {
            if (page.download_id === folder_id) {
                Controller.getItemsInFolder(page.download_id, filesModel)
            }
        }
    }

    Connections {
        target: Controller
        onItemsLoaded: {
            loadingIndicator.running = false
            loadingIndicator.visible = false
            foldersView.visible = true
            filesView.visible = true
        }
    }

    Component.onCompleted: {
        Controller.getFolders(page.download_id, foldersModel)
    }

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        anchors.fill: parent

        Column {
            anchors.fill: parent
            spacing: 0

            SilicaListView {
                id: foldersView

                model: foldersModel

                width: parent.width
                height: contentHeight

                spacing: 0

                header: PageHeader {
                    id: title
                    title: dir_name
                }

                delegate: DictionaryPictureItem {
                    onClicked: {
                        changeFolderForCover(model.id)
                        pageStack.animatorPush(Qt.resolvedUrl("../pages/SecondPage.qml"), {download_id: model.id, dir_name: model.name})
                    }
                }

                VerticalScrollDecorator {}
            }

            SilicaListView {
                id: filesView

                model: filesModel

                width: parent.width
                height: contentHeight

                spacing: 0

                delegate: DictionaryPictureItem {
                    onClicked: {
                        pageStack.animatorPush(Qt.resolvedUrl("../pages/PicturePage.qml"), {download_id: download_id, started_index: index})
                    }
                }

                VerticalScrollDecorator {}
            }
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
