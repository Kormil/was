import QtQuick 2.0
import Sailfish.Silica 1.0

import Settings 1.0
import FileListModel 1.0
import Controller 1.0

import "../items"

Page {
    property bool logging: false;
    property int id: 0;
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
        onLoginCompleted: {
            loadingIndicator.running = false
            loadingIndicator.visible = false
            loginLabel.visible = false
            logging = false

            Controller.clear(foldersModel)
            Controller.getRootFolder()

            model_for_cover_id: 0
        }
    }

    Connections {
        target: Settings
        onApiSpaceChanged: {
            Controller.clear(foldersModel)
            Controller.getRootFolder()
        }
    }

    Connections {
        target: Controller
        onLoginStarted: {
            loadingIndicator.running = true
            loadingIndicator.visible = true
            loginLabel.visible = true
            logging = true
        }
    }

    Connections {
        target: Controller
        onRootFolderIdLoading: {
            loadingIndicator.running = true
            loadingIndicator.visible = true
            loginLabel.visible = false
            foldersView.visible = false
            filesView.visible = false
        }
    }

    Connections {
        target: Controller
        onRootFolderIdLoaded: {
            page.id = folder_id
            Controller.getFolders(page.id, foldersModel)
        }
    }

    Connections {
        target: Controller
        onFoldersLoaded: {
            if (folder_id === page.id) {
                Controller.getItemsInFolder(folder_id, filesModel)
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

    Connections {
        target: Settings
        onPasswordReaded: {
            Controller.login(Settings.quickconnect, Settings.userLogin, Settings.password)
        }
    }

    Connections {
        target: Settings
        onInitialized: {
            if (Settings.autologin) {
                Settings.loadPassword()
            }
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
                text: qsTr("About Was")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
                }
            }

            MenuItem {
                visible: !Controller.logged && logging === false

                text: qsTr("Login")
                onClicked: pageStack.animatorPush(Qt.resolvedUrl("../dialogs/LoginDialog.qml"), {foldersModel: foldersModel})
            }

            MenuItem {
                visible: Controller.logged && logging === false

                text: qsTr("Logout")
                onClicked: Controller.logout()
            }

            MenuItem {
                visible: Settings.api_space === 0 && Controller.logged && logging === false
                text: qsTr("Change to shared space")
                onClicked: {
                    Settings.api_space = 1
                }
            }

            MenuItem {
                visible: Settings.api_space === 1 && Controller.logged && logging === false
                text: qsTr("Change to personal space")
                onClicked: {
                    Settings.api_space = 0
                }
            }
        }

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
                        if (!Controller.logged) {
                            pageStack.animatorPush(Qt.resolvedUrl("../dialogs/LoginDialog.qml"), {foldersModel: foldersModel})
                        } else {
                            changeFolderForCover(model.id)
                            pageStack.animatorPush(Qt.resolvedUrl("../pages/SecondPage.qml"), {download_id: model.id, dir_name: model.name})
                        }
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
                        if (!Controller.logged) {
                            pageStack.animatorPush(Qt.resolvedUrl("../dialogs/LoginDialog.qml"), {foldersModel: foldersModel})
                        } else {
                            pageStack.animatorPush(Qt.resolvedUrl("../pages/PicturePage.qml"), {download_id: page.id, started_index: index})
                        }
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
