#include "controller.h"
#include <QtQml>

Controller::Controller(QObject *parent) : QObject(parent) {
    connection_ = std::make_shared<Connection>();
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

void Controller::bindToQml(QQuickView *view)
{
    qmlRegisterSingletonType<Controller>("Controller", 1, 0, "Controller", Controller::instance);
}


void Controller::login(QString quickconnect, QString login, QString password) {
    emit loginStarted();

    connection_->login(quickconnect, login, password, [this](bool result){
        loginResult = result;
        emit loginCompleted();
    });
}

bool Controller::getLoginResult() {
    return loginResult;
}
