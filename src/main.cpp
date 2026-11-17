#include <QApplication>
#include <QGuiApplication>
#include <QQuickView>
#include <QSettings>
#include <QDebug>
#include <QUrl>
#include <QQmlApplicationEngine>

#include "view/mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("TecnologiaObjetos");
    QCoreApplication::setApplicationName("appTecnologiaObjetos");

    // Firebase
    QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini",
                       QSettings::IniFormat);

    qDebug() << QCoreApplication::applicationDirPath();
    QString apiKey = settings.value("FIREBASE_API_KEY").toString();
    QString projectId = settings.value("FIREBASE_PROJECT_ID").toString();

    MainWindow w;
    w.setApiConfig(apiKey, projectId);
    w.show();

    QQmlApplicationEngine engine;
    // Load the QML file from the resource system
    const QUrl url = QUrl("qrc:/traficMap.qml");

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &a, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.load(url);

    return a.exec();
}
