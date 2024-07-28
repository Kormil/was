#ifndef SETTINGS_H
#define SETTINGS_H

#include <memory>
#include <QObject>
#include <QVariant>
#include <QSettings>

#include "secretsmanager.h"
#include "src/connection/request.h"

class QSettings;
class QJSEngine;
class QQmlEngine;

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autologin READ autologin WRITE setAutologin NOTIFY autologinChanged)
    Q_PROPERTY(QString quickconnect READ quickconnect WRITE setQuickconnect NOTIFY quickconnectChanged)
    Q_PROPERTY(QString userLogin READ userLogin WRITE setUserLogin NOTIFY loginChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordReaded)
    Q_PROPERTY(Request::Space api_space READ apiSpace WRITE setApiSpace NOTIFY apiSpaceChanged)

public:
    static QObject *instance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static Settings *instance();
    static void bindToQml();

    Q_INVOKABLE QString license();

    // login part
    bool autologin() const;
    void setAutologin(bool value);

    QString quickconnect() const;
    void setQuickconnect(const QString& value);

    QString userLogin() const;
    void setUserLogin(const QString& value);

    QString password() const;
    void setPassword(const QString& value);

    Request::Space apiSpace() const;
    void setApiSpace(Request::Space value);

    Q_INVOKABLE void loadPassword();
    Q_INVOKABLE void saveSettings();

signals:
    void autologinChanged();
    void quickconnectChanged();
    void loginChanged();

    void initialized();

    void settingsSaved();
    void passwordReaded();

    void apiSpaceChanged();
private:
    explicit Settings(QObject *parent = nullptr);
    std::unique_ptr<QSettings> m_settings;

    QString quickconnect_;
    QString user_login_;
    QString user_password_;

    Request::Space api_space_;

    SecretsManager secrets_manager_;
};

#endif // SETTINGS_H
