#include "settings.h"

#include <QtQml>
#include <QVariant>
#include <sailfishapp.h>

#define LOGIN_AUTOLOGIN_SETTINGS_PATH QStringLiteral("login/autologin")
#define LOGIN_QUICKCONNECT_SETTINGS_PATH QStringLiteral("login/quickconnect")
#define LOGIN_USERLOGIN_SETTINGS_PATH QStringLiteral("login/userlogin")
#define LOGIN_PASSWORD_SETTINGS_PATH QStringLiteral("login/password")
#define API_SPACE_SETTINGS_PATH QStringLiteral("api/space")

namespace {
int API_PERSONAL_SPACE = 0;
}

Settings::Settings(QObject *parent) {
    const QString settingsPath =
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
        + "/" + QCoreApplication::applicationName() + ".conf";
    m_settings = std::make_unique<QSettings>(settingsPath, QSettings::NativeFormat);

    secrets_manager_.isCollectionExistsInSecretsManager([this](){
        emit initialized();
    });

    api_space_= m_settings->value(API_SPACE_SETTINGS_PATH, API_PERSONAL_SPACE).value<Request::Space>();
}

bool Settings::autologin() const
{
    return m_settings->value(LOGIN_AUTOLOGIN_SETTINGS_PATH, false).toBool();
}

void Settings::setAutologin(bool value)
{
    if (autologin() != value) {
        m_settings->setValue(LOGIN_AUTOLOGIN_SETTINGS_PATH, value);
        emit autologinChanged();
    }
}

QString Settings::quickconnect() const
{
    return m_settings->value(LOGIN_QUICKCONNECT_SETTINGS_PATH, "").toString();
}

void Settings::setQuickconnect(const QString& value)
{
    if (quickconnect_ != value) {
        quickconnect_ = value;
//        m_settings->setValue(LOGIN_QUICKCONNECT_SETTINGS_PATH, value);
        emit quickconnectChanged();
    }
}

QString Settings::userLogin() const
{
    return m_settings->value(LOGIN_USERLOGIN_SETTINGS_PATH, "").toString();
}

void Settings::setUserLogin(const QString &value)
{
    if (user_login_ != value) {
        user_login_ = value;
//        m_settings->setValue(LOGIN_USERLOGIN_SETTINGS_PATH, value);
        emit loginChanged();
    }
}

QString Settings::password() const {
    return user_password_;
}

void Settings::loadPassword() {
    secrets_manager_.readFromSecretsManager(LOGIN_PASSWORD_SETTINGS_PATH, [this](QString value) {
        setPassword(value);
    });
}

void Settings::setPassword(const QString& value) {
    if (user_password_ != value) {
        user_password_ = value;

        emit passwordReaded();
    }
}

QObject *Settings::instance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return instance();
}

Settings *Settings::instance()
{
    static Settings instance;
    return &instance;
}

QString Settings::license()
{
    QString licenseFile = SailfishApp::pathTo(QString("LICENSE")).toLocalFile();

    if (!QFile::exists(licenseFile)) {
        return "License not found.";
    }

    QFile file(licenseFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "Could not open: " + licenseFile;

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return content;
}

void Settings::bindToQml()
{
    qmlRegisterSingletonType<Settings>("Settings", 1, 0, "Settings", Settings::instance);
}

void Settings::saveSettings() {
    m_settings->setValue(LOGIN_QUICKCONNECT_SETTINGS_PATH, quickconnect_);
    m_settings->setValue(LOGIN_USERLOGIN_SETTINGS_PATH, user_login_);

    secrets_manager_.deleteFromSecretsManager(LOGIN_PASSWORD_SETTINGS_PATH, [this](bool can_save) {
        if (can_save) {
            secrets_manager_.saveToSecretsManager(LOGIN_PASSWORD_SETTINGS_PATH, user_password_);
        }
    });
}

Request::Space Settings::apiSpace() const
{
    return api_space_;
}

void Settings::setApiSpace(Request::Space value)
{
    if (apiSpace() != value) {
        m_settings->setValue(API_SPACE_SETTINGS_PATH, value);
        api_space_ = value;

        emit apiSpaceChanged();
    }
}
