#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QQuickView>
#include <QQmlEngine>
#include <QJSEngine>

#include "connection.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    static Controller& instance();
    static QObject *instance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static void bindToQml(QQuickView *view);

    Q_INVOKABLE void login(QString quickconnect, QString login, QString password);
    Q_INVOKABLE bool getLoginResult();

signals:
    void loginStarted();
    void loginCompleted();

private:
    explicit Controller(QObject *parent = nullptr);

    bool loginResult = false;

    ConnectionPtr connection_;
};

#endif // CONTROLLER_H
