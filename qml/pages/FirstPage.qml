import QtQuick 2.0
import Sailfish.Silica 1.0

import FileListModel 1.0
import Controller 1.0

Page {
    property int download_id: 0;

    id: page

    FileListModel {
        id: fileListModel
    }

    Component.onCompleted: {
        if (page.download_id !== 0) {
            Controller.contentOfPhotoDirectory(page.download_id, fileListModel)
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

        //contentHeight: listView.height

        SilicaListView {
            id: listView

            model: fileListModel

            anchors.fill: parent
            spacing: Theme.paddingMedium

            header: PageHeader {
                id: title
                title: qsTr("Photos")
            }

            delegate: ListItem {
                contentHeight: Theme.itemSizeSmall

                Rectangle {
                    anchors.fill: parent
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightBackgroundColor, 0.10) }
                        GradientStop { position: 1.0; color: "transparent" }
                    }
                }

                Label {
                    id: fileLabel
                    text: model.name
                    height: Theme.itemSizeSmall
                    anchors.centerIn: parent
                }

                onClicked: {
                    console.info(model.id)
                    var new_page = pageStack.animatorPush(Qt.resolvedUrl("../pages/FirstPage.qml"), {download_id: model.id})
                }
            }

            VerticalScrollDecorator {}
        }
    }
}
