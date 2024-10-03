#include "controller.h"
#include <QtQml>

#include "src/models/filelistmodel.h"
#include "src/models/showpicturemodel.h"
#include "src/models/folderlistmodel.h"
#include "src/settings.h"
#include "src/connection/requests/loginrequest.h"
#include "src/connection/requests/logoutrequest.h"
#include "src/connection/requests/getthumbnailrequest.h"
#include "src/connection/requests/getrootfoldersrequest.h"

Q_DECLARE_METATYPE(FileListModel*)
Q_DECLARE_METATYPE(ShowPictureModel*)

Controller::Controller(QObject *parent) : QObject(parent) {
    connection_ = std::make_shared<Connection>();

    files_manager_ = std::make_unique<FilesManager>(connection_);
}

Controller& Controller::instance()
{
    static Controller instance;
    return instance;
}

QObject *Controller::instance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return &instance();
}

void Controller::bindToQml()
{
    qmlRegisterSingletonType<Controller>("Controller", 1, 0, "Controller", Controller::instance);

    // files
    qRegisterMetaType<FileListModel*>();
    qmlRegisterType<FileListModel>("FileListModel", 1, 0, "FileListModel");

    qRegisterMetaType<ShowPictureModel*>();
    qmlRegisterType<ShowPictureModel>("ShowPictureModel", 1, 0, "ShowPictureModel");

    qRegisterMetaType<FolderListModel*>();
    qmlRegisterType<FolderListModel>("FolderListModel", 1, 0, "FolderListModel");


    //qmlRegisterType<Dir>("Dir", 1, 0, "Dir");
}

void Controller::login(const QString &quickconnect, const QString &login, const QString &password) {
    emit loginStarted();

    connection_->setQuickconnect(quickconnect);

    auto handler = [this](bool result, const QString &sid) {
        connection_->setSid(sid);
        loginResult = result;

        emit loginStatusChanged();
        emit loginCompleted(result);
    };

    auto request = std::make_shared<LoginRequest>(quickconnect, login, password, handler);
    auto serial = connection_->addRequest(request);

    connection_->runRequest(serial);
}

bool Controller::getLoginResult() {
    return loginResult;
}

void Controller::clear(FileListModel* file_list_model) {
    files_manager_->clear();
}

void Controller::getRootFolder() {
    emit rootFolderIdLoading();

    auto space = Settings::instance()->apiSpace();

    auto request = std::make_shared<GetRootFoldersRequest>(space, [this](int id) {
        emit rootFolderIdLoaded(id);
    });

    auto serial = connection_->addRequest(request);
    connection_->runRequest(serial);
}

void Controller::getFolderSize(int id) {
    files_manager_->getFolderSize(id);
}

FileListPtr Controller::getFolders(IdType id) {
    return files_manager_->getFolders(id);
}

FileListPtr Controller::getItemsInFolder(IdType id) {
    return files_manager_->getItems(id);
}

void Controller::getThumbnail(const IdType &id, const QString &cacheKey, const QString &type) {
    auto space = Settings::instance()->apiSpace();

    auto request = std::make_shared<GetThumbnailRequest>(Image::Size::SM, id, cacheKey, type, space, [this, cacheKey](const QImage& image) {
        emit onGetThumbnailFinished(cacheKey, image);
    });

    auto serial = connection_->addRequest(request);
    connection_->runRequest(serial);
}

void Controller::getImage(const IdType &id, const QString &cacheKey, const QString &type) {
    auto space = Settings::instance()->apiSpace();

    auto request = std::make_shared<GetThumbnailRequest>(Image::Size::XL, id, cacheKey, type, space, [this, cacheKey](const QImage& image) {
        emit onGetImageFinished(cacheKey, image);
    });

    auto serial = connection_->addRequest(request);
    connection_->runRequest(serial);
}

void Controller::logout() {
    auto handler = [this]() {
        loginResult = false;

        emit loginStatusChanged();
    };

    auto request = std::make_shared<LogoutRequest>(handler);
    auto serial = connection_->addRequest(request);

    connection_->runRequest(serial);
}
