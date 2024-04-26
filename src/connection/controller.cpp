#include "controller.h"
#include <QtQml>

Q_DECLARE_METATYPE(FileListModel*)

namespace {
    IdType RootId = 0;
};

Controller::Controller(QObject *parent) : QObject(parent) {
    connection_ = std::make_shared<Connection>();

    file_list_model_ = std::make_shared<FileListModel>();
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
    //qmlRegisterType<Dir>("Dir", 1, 0, "Dir");
}

void Controller::login(FileListModel* file_list_model, QString quickconnect, QString login, QString password) {
    emit loginStarted();

    connection_->login(quickconnect, login, password, [this, file_list_model](bool result){
        loginResult = result;
        contentOfPhotoDirectory(RootId, file_list_model);

        emit loginCompleted();
    });
}

bool Controller::getLoginResult() {
    return loginResult;
}

void Controller::contentOfPhotoDirectory(int id, FileListModel* file_list_model) {
//    connection_->contentOfPhotoDirectory([this](FileListPtr file_list) {
//        file_list_model_->setList(file_list);
//    });

    files_manager_->get(id, [this, file_list_model](FileListPtr file_list) {
        file_list_model->setList(file_list);
    });
}
