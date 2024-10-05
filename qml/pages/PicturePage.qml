import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Gallery 1.0

import FileListModel 1.0
import ShowPictureModel 1.0
import Controller 1.0

import "../dialogs"
import "../cover"

Page {
    property int download_id: 0
    property int started_index: 0

    id: page

    allowedOrientations: Orientation.All

    Component.onCompleted: {
        changeCoverPage(picture_cover_component)
    }

    ShowPictureModel {
        id: fileListModel
        folder_id: download_id
    }

    onStatusChanged: {
        if (page.status === PageStatus.Active) {
            page.navigationStyle = PageNavigation.Vertical
        }
    }

    Rectangle {
        id: background

        anchors.fill: parent
        color: Theme.colorScheme == Theme.LightOnDark ? "black" : "white"
    }


    Binding {
        target: fileListModel;
        property: "current_index";
        value: listView.currentIndex
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
                id: mainImage
                source: model.photo_source
                anchors.fill: parent
            }
        }
    }

    Component {
        id: picture_cover_component

        PictureCover {
            id: picture_cover
            picture_source: fileListModel.current_picture_source
        }
    }
}
