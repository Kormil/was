import QtQuick 2.0
import Sailfish.Silica 1.0

import Controller 1.0

Dialog {
    id: page

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    acceptDestination: Qt.resolvedUrl("../dialogs/SaveLoginDataDialog.qml")
    acceptDestinationAction: PageStackAction.Push

    SilicaListView {
        id: listView
        anchors.fill: parent

        Column {
            width: parent.width

//            PageHeader {
//                title: qsTr("Login")
//            }

            TextField {
                id: quickconnect
                width: parent.width
                placeholderText: qsTr("Quickconnect")
                label: qsTr("Quickconnect")
            }

            TextField {
                id: loginField
                width: parent.width
                placeholderText: qsTr("login")
                label: qsTr("login")
            }

            TextField {
                id: passwordField
                width: parent.width
                placeholderText: qsTr("password")
                label: qsTr("password")
                echoMode: TextInput.Password
            }

//            Button {
//                anchors.horizontalCenter: parent.horizontalCenter
//                text: "Login"
//                onClicked: {
//                    Controller.login(quickconnect.text, loginField.text, passwordField.text)
//                }
//            }

//            //Row {
//                spacing: 1
//                Text {
//                    id: statusLabel
//                    width: parent.width
//                    font.pixelSize: Theme.fontSizeExtraSmall
//                    color: Theme.secondaryColor
//                    text: "Success? "
//                    wrapMode: Text.Wrap
//                }

//                Text {
//                    id: statusText
//                    width: parent.width
//                    font.pixelSize: Theme.fontSizeExtraSmall
//                    color: Theme.secondaryColor
//                    text: "Not logged"
//                    wrapMode: Text.Wrap
//                }
//            //}
        }

        VerticalScrollDecorator {}
    }

//    Connections {
//        target: Controller
//        onLoginCompleted: {
//            statusText.text = Controller.getLoginResult()
//        }
//    }
}
