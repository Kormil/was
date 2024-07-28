#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QQuickView>
#include <QQmlEngine>
#include <QJSEngine>

#include "connection.h"

#include "src/types/file.h"
#include "src/types/filesmanager.h"

class FileListModel;

class Controller : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool logged READ getLoginResult NOTIFY loginStatusChanged)

public:
    static Controller& instance();
    static QObject *instance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static void bindToQml();

    Q_INVOKABLE void login(const QString &quickconnect, const QString &login, const QString &password);
    Q_INVOKABLE void logout();
    Q_INVOKABLE bool getLoginResult();

    Q_INVOKABLE void clear(FileListModel* file_list_model);
    Q_INVOKABLE void contentOfPhotoDirectory(int id, unsigned int start_point, FileListModel *file_list_model);


public slots:
    void getThumbnail(const IdType &id, const QString &cacheKey, const QString &type);
    void getImage(const IdType &id, const QString &cacheKey, const QString &type);

signals:
    void loginStarted();
    void loginCompleted();
    void loginStatusChanged();

    void photosLoading();
    void photosLoaded();

    void onGetThumbnailFinished(const QString &, const QImage &);
    void onGetImageFinished(const QString &, const QImage &);

private:
    explicit Controller(QObject *parent = nullptr);

    bool loginResult = false;

    ConnectionPtr connection_;

    FilesManagerPtr files_manager_;
};

#endif // CONTROLLER_H
