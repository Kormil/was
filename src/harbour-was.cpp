#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QGuiApplication>
#include <sailfishapp.h>

#include "src/settings.h"
#include "src/connection/asyncimageprovider.h"
#include "src/connection/controller.h"
#include "src/connection/networkaccessmanagerfactory.h"

int main(int argc, char *argv[])
{
    QGuiApplication * app = SailfishApp::application(argc, argv);
    QQuickView * view = SailfishApp::createView();

    NetworkAccessManagerFactory factory;

    Controller &controller = Controller::instance();
    controller.bindToQml();

    Settings::bindToQml();

    view->engine()->addImageProvider("photos", new AsyncImageProvider(controller));
    view->engine()->setNetworkAccessManagerFactory(&factory);
    view->setSource(SailfishApp::pathToMainQml());
    view->show();
    int result = app->exec();

    view->destroy();

    return result;
}
