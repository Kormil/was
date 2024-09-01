import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"
import "cover"

ApplicationWindow {
    property int model_for_cover_id: 0;

    function changeCoverPage(coverUrl) {
        if (coverUrl !== cover) {
            cover = coverUrl
        }

        return cover
    }

    function resetCoverPage() {
        changeCoverPage(folder_cover_component)
    }

    function changeFolderForCover(folder_id) {
        model_for_cover_id = folder_id
    }

    initialPage: Component { FirstPage { } }
    cover: folder_cover_component
    allowedOrientations: defaultAllowedOrientations

    Component {
        id: folder_cover_component

        CoverPage {
            id: folder_cover
            download_id: model_for_cover_id
        }
    }
}
