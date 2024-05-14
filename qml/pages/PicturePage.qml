import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Gallery 1.0

import FileListModel 1.0
import Controller 1.0

import "../dialogs"

Page {
    property int download_id: 0;
    property int started_index: 0;

    FileListModel {
        id: fileListModel
    }

    id: page

    Connections {
        target: page
        Component.onCompleted: {
            Controller.contentOfPhotoDirectory(page.download_id, fileListModel)
        }
    }

    onStatusChanged: {
        if (page.status === PageStatus.Active) {
            page.navigationStyle = PageNavigation.Vertical
        }
    }

    Rectangle {
        id: background

        anchors.fill: parent
        color: "black";
    }

    SilicaListView {
        id: listView
        model: fileListModel

        clip: true
        snapMode: ListView.SnapOneItem
        orientation: ListView.HorizontalFlick
        highlightRangeMode: ListView.StrictlyEnforceRange
        cacheBuffer: width

        currentIndex: started_index
        anchors.fill: parent

        delegate: Item {
            width: listView.width
            height: listView.height
            clip: true

            ImageViewer {
                source: model.photo_source
                anchors.fill: parent
            }
        }
    }
}
