project(harbour-was CXX)
cmake_minimum_required(VERSION 3.5)

find_package (Qt5 COMPONENTS Core Network Qml Gui Quick LinguistTools REQUIRED)

include(FindPkgConfig)
pkg_search_module(SAILFISH sailfishapp REQUIRED)
pkg_search_module(SAILFISH sailfishsecrets REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(WAS_SOURCES
    src/connection/connection.cpp
    src/connection/connection.h
    src/connection/controller.cpp
    src/connection/controller.h
    src/connection/requests/loginrequest.cpp
    src/connection/requests/loginrequest.h
    src/connection/requests/logoutrequest.cpp
    src/connection/requests/logoutrequest.h
    src/connection/requests/getfoldersrequest.cpp
    src/connection/requests/getfoldersrequest.h
    src/connection/requests/getrootfoldersrequest.cpp
    src/connection/requests/getrootfoldersrequest.h
    src/connection/requests/getitemsrequest.cpp
    src/connection/requests/getitemsrequest.h
    src/connection/requests/countfoldersrequest.cpp
    src/connection/requests/countfoldersrequest.h
    src/connection/requests/countitemsrequest.cpp
    src/connection/requests/countitemsrequest.h
    src/connection/requests/getthumbnailrequest.cpp
    src/connection/requests/getthumbnailrequest.h
    src/connection/request.cpp
    src/connection/request.h
    src/connection/asyncimageprovider.h
    src/types/dir.cpp
    src/types/dir.h
    src/types/image.cpp
    src/types/image.h
    src/types/file.h
    src/types/filelist.cpp
    src/types/filelist.h
    src/types/filesmanager.cpp
    src/types/filesmanager.h
    src/models/folderlistmodel.cpp
    src/models/folderlistmodel.h
    src/models/filelistmodel.cpp
    src/models/filelistmodel.h
    src/models/showpicturemodel.cpp
    src/models/showpicturemodel.h
    src/settings.cpp
    src/settings.h
    src/secretsmanager.cpp
    src/secretsmanager.h
    src/harbour-was.cpp
)

set(KORPHO_QML_SOURCES
    qml/harbour-was.qml
    qml/cover/CoverPage.qml
    qml/cover/PictureCover.qml
    qml/pages/FirstPage.qml
    qml/pages/SecondPage.qml
    qml/pages/AboutPage.qml
    qml/pages/LicensePage.qml
    qml/pages/PicturePage.qml
    qml/pages/SettingsPage.qml
    qml/dialogs/LoginDialog.qml
    qml/dialogs/SaveLoginDataDialog.qml
    qml/dialogs/AutoLoginOnStartDialog.qml
    qml/items/DictionaryPictureItem.qml
    qml/items/AnimatedImage.qml
    )

qt5_add_resources(graphics.qrc)

add_executable(harbour-was ${WAS_SOURCES} ${KORPHO_QML_SOURCES} graphics.qrc)
target_compile_definitions(harbour-was PRIVATE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)
target_include_directories(harbour-was PRIVATE
    "/usr/include/Sailfish"
    $<BUILD_INTERFACE:
    ${SAILFISH_INCLUDE_DIRS}
>)
target_link_libraries(harbour-was
    Qt5::Quick
    ${SAILFISH_LDFLAGS}
    sailfishsecrets
)

install(TARGETS harbour-was
    RUNTIME DESTINATION bin
)
install(DIRECTORY qml
    DESTINATION share/harbour-was
)
install(DIRECTORY translations
    DESTINATION share/harbour-was
    FILES_MATCHING PATTERN "*.qm"
)
install(FILES harbour-was.desktop
    DESTINATION share/applications
)
install(FILES icons/86x86/harbour-was.png
    DESTINATION share/icons/hicolor/86x86/apps
)
install(FILES icons/108x108/harbour-was.png
    DESTINATION share/icons/hicolor/108x108/apps
)
install(FILES icons/128x128/harbour-was.png
    DESTINATION share/icons/hicolor/128x128/apps
)
install(FILES icons/172x172/harbour-was.png
    DESTINATION share/icons/hicolor/172x172/apps
)
install(FILES LICENSE
    DESTINATION share/harbour-was
)

# Get the other files reachable from the project tree in Qt Creator
FILE(GLOB TsFiles "translations/*.ts")
FILE(GLOB QM_FILES "translations/*.qm")

add_custom_target(distfiles
    SOURCES
        harbour-was.desktop
        ${KORPHO_QML_SOURCES}
        rpm/harbour-was.changes.in
        rpm/harbour-was.changes.run.sin
        rpm/harbour-was.spec
        rpm/harbour-was.yaml
        graphics.qrc
        LICENSE
        translations/harbour-was.ts
        ${QM_FILES}
        ${TsFiles})

# Tell Qt Creator where the application executable(s) would be located on the
# device.
#
# It is not necessary to list other deployables than executables (runtime
# targets) here. The deployment process of Sailfish OS projects is opaque to
# Qt Creator and the information contained in QtCreatorDeployment.txt is only
# used to locate the executable associated with the active run configuration
# on the device in order to run it.
#
# Search the Qt Creator Manual to learn about the QtCreatorDeployment.txt file
# format.
file(WRITE "${CMAKE_BINARY_DIR}/QtCreatorDeployment.txt"
    "${CMAKE_INSTALL_PREFIX}
${CMAKE_BINARY_DIR}/harbour-was:bin
")

