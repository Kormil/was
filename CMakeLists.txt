project(harbour-was CXX)
cmake_minimum_required(VERSION 3.5)

find_package (Qt5 COMPONENTS Core Network Qml Gui Quick REQUIRED)

include(FindPkgConfig)
pkg_search_module(SAILFISH sailfishapp REQUIRED)
pkg_search_module(SAILFISH sailfishsecrets REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(KORPHO_SOURCES
    src/connection/connection.cpp
    src/connection/controller.cpp
    src/connection/request.cpp
    src/settings.cpp
    src/secretsmanager.cpp
    src/harbour-was.cpp
)

add_executable(harbour-was ${KORPHO_SOURCES})
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

# Get the other files reachable from the project tree in Qt Creator
FILE(GLOB TsFiles "translations/*.ts")
add_custom_target(distfiles
    SOURCES
        harbour-was.desktop
        qml/harbour-was.qml
        qml/cover/CoverPage.qml
        qml/pages/FirstPage.qml
        qml/dialogs/LoginDialog.qml
        qml/dialogs/SaveLoginDataDialog.qml
        qml/dialogs/AutoLoginOnStartDialog.qml
        rpm/harbour-was.changes.in
        rpm/harbour-was.changes.run.in
        rpm/harbour-was.spec
        rpm/harbour-was.yaml
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

