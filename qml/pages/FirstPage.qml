import QtQuick 2.0
import Sailfish.Silica 1.0

import FileListModel 1.0
import Controller 1.0

import "../items"

Page {
    property int download_id: 0;
    property string dir_name: qsTr("Photos")

    id: page

    FileListModel {
        id: fileListModel
    }

    Connections {
        target: Controller
        onLoginCompleted: {
            Controller.contentOfPhotoDirectory(0, 0, fileListModel)
        }
    }

    Component.onCompleted: {
        if (page.download_id !== 0) {
            Controller.contentOfPhotoDirectory(page.download_id, 0, fileListModel)
        }
    }

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        anchors.fill: parent

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                text: qsTr("Login")
                onClicked: pageStack.animatorPush(Qt.resolvedUrl("../dialogs/LoginDialog.qml"), {fileListModel: fileListModel})
            }
        }

        SilicaListView {
            id: listView

            model: fileListModel

            anchors.fill: parent
            spacing: 0

            header: PageHeader {
                id: title
                title: dir_name
            }

            delegate: DictionaryPictureItem {
                onClicked: {
                    if (model.is_dir === true) {
                        pageStack.animatorPush(Qt.resolvedUrl("../pages/FirstPage.qml"), {download_id: model.id, dir_name: model.name})
                    } else {
                        pageStack.animatorPush(Qt.resolvedUrl("../pages/PicturePage.qml"), {download_id: download_id, started_index: index})
                    }
                }
            }

            VerticalScrollDecorator {}
        }
    }
}
