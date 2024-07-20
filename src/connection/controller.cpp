#include "controller.h"
#include <QtQml>

#include "src/models/filelistmodel.h"
#include "src/models/showpicturemodel.h"

Q_DECLARE_METATYPE(FileListModel*)
Q_DECLARE_METATYPE(ShowPictureModel*)

namespace {
    IdType RootId = 0;
};

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

    //qmlRegisterType<Dir>("Dir", 1, 0, "Dir");
}

void Controller::login(QString quickconnect, QString login, QString password) {
    emit loginStarted();

    connection_->login(quickconnect, login, password, [this](bool result) {
        loginResult = result;

        emit loginCompleted();
    });
}

bool Controller::getLoginResult() {
    return loginResult;
}

void Controller::contentOfPhotoDirectory(int id, unsigned int start_point, FileListModel* file_list_model) {
    emit photosLoading();

    files_manager_->get(id, start_point, [this, file_list_model](const FileListPtr& file_list) {
        file_list_model->setList(file_list);
        files_manager_->getItemsCounter(file_list);

        emit photosLoaded();
    });
}

void Controller::getThumbnail(const IdType &id, const QString &cacheKey, const QString &type) {
    connection_->getThumbnail(Image::Size::SM, id, cacheKey, type, [this, cacheKey](const QImage& image) {
        emit onGetThumbnailFinished(cacheKey, image);
    });
}

void Controller::getImage(const IdType &id, const QString &cacheKey, const QString &type) {
    connection_->getThumbnail(Image::Size::XL, id, cacheKey, type, [this, cacheKey](const QImage& image) {
        emit onGetImageFinished(cacheKey, image);
    });
}
