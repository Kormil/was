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
            loadingIndicator.running = false
            loadingIndicator.visible = false
            loginLabel.visible = false

            Controller.contentOfPhotoDirectory(0, 0, fileListModel)
        }
    }

    Connections {
        target: Controller
        onLoginStarted: {
            loadingIndicator.running = true
            loadingIndicator.visible = true
            loginLabel.visible = true
        }
    }

    Connections {
        target: Controller
        onPhotosLoading: {
            loadingIndicator.running = true
            loadingIndicator.visible = true
            loginLabel.visible = false
        }
    }

    Connections {
        target: Controller
        onPhotosLoaded: {
            loadingIndicator.running = false
            loadingIndicator.visible = false
            loginLabel.visible = false
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
            visible: download_id === 0

            MenuItem {
                visible: !Controller.logged

                text: qsTr("Login")
                onClicked: pageStack.animatorPush(Qt.resolvedUrl("../dialogs/LoginDialog.qml"), {fileListModel: fileListModel})
            }

            MenuItem {
                visible: Controller.logged

                text: qsTr("Logout")
                onClicked: Controller.logout()
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
                    if (!Controller.logged) {
                        pageStack.animatorPush(Qt.resolvedUrl("../dialogs/LoginDialog.qml"), {fileListModel: fileListModel})
                    }

                    if (model.is_dir === true) {
                        pageStack.animatorPush(Qt.resolvedUrl("../pages/FirstPage.qml"), {download_id: model.id, dir_name: model.name})
                    } else {
                        pageStack.animatorPush(Qt.resolvedUrl("../pages/PicturePage.qml"), {download_id: download_id, started_index: index})
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

        Label {
            id: loginLabel
            text: qsTr("Logging in")
            anchors.top: loadingIndicator.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            visible: false
            color: Theme.highlightColor
        }
    }
}
