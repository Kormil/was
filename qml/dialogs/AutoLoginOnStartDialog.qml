import QtQuick 2.0
import Sailfish.Silica 1.0

import Controller 1.0
import Settings 1.0

Dialog {
    id: page

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    onAccepted: {
        Settings.autologin = true
    }

    onCanceled: {
        Settings.autologin = false
    }

    SilicaListView {
        id: listView
        anchors.fill: parent

        DialogHeader {
            id: dialogHeader

            acceptText: qsTr("Yes")
            cancelText: qsTr("No")
        }

        Column {
            anchors.top: dialogHeader.bottom
            width: parent.width

            Label {
                text: qsTr("Do you want to log in at the start?")
                anchors.top: header.bottom
                x: Theme.horizontalPageMargin
                color: Theme.highlightColor
            }
        }

        VerticalScrollDecorator {}
    }
}
