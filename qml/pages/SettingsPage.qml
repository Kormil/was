import QtQuick 2.0
import Sailfish.Silica 1.0
import Settings 1.0

Page {
    id: page

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent

        Column {
            id: column
            anchors.fill: parent

            PageHeader {
                title: qsTr("Settings")
            }

            TextSwitch {
                id: alwaysLargeImagesTextSwitch
                text: qsTr("Login it at startup")
                checked: Settings.autologin

                onCheckedChanged: {
                    Settings.autologin = checked
                }
            }
        }

        VerticalScrollDecorator {}
    }
}
