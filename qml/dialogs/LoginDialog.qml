import QtQuick 2.0
import Sailfish.Silica 1.0

import Controller 1.0
import Settings 1.0

Dialog {
    id: page

    Component.onCompleted: {
        Settings.loadPassword()
    }

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    acceptDestination: Qt.resolvedUrl("../dialogs/SaveLoginDataDialog.qml")
    acceptDestinationAction: PageStackAction.Replace

    onAccepted: {
        Settings.quickconnect = quickconnect.text
        Settings.userLogin = loginField.text
        Settings.password = passwordField.text
    }

    SilicaListView {
        id: listView
        anchors.fill: parent

        DialogHeader {
            id: dialogHeader
            acceptText: qsTr("Login")
        }

        Column {
            anchors.top: dialogHeader.bottom
            width: parent.width

            TextField {
                id: quickconnect
                width: parent.width
                placeholderText: qsTr("QuickConnect")
                label: qsTr("QuickConnect")
                text: Settings.quickconnect
            }

            TextField {
                id: loginField
                width: parent.width
                placeholderText: qsTr("login")
                label: qsTr("login")
                text: Settings.userLogin
            }

            TextField {
                id: passwordField
                width: parent.width
                placeholderText: qsTr("password")
                label: qsTr("password")
                echoMode: TextInput.Password
                text: Settings.password
            }

//            Button {
//                anchors.horizontalCenter: parent.horizontalCenter
//                text: "Login"
//                onClicked: {
//                    console.debug(Settings.password)
//                   // Controller.login(quickconnect.text, loginField.text, passwordField.text)
//                }
//            }
        }

        VerticalScrollDecorator {}
    }

    Connections {
        target: Settings
        onInitialized: {
            Settings.loadPassword()
        }
    }
}