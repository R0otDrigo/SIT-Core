#include <QApplication>
#include <QSettings>
#include <QDebug>

#include "mainwindow.h"

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
    return a.exec();
}
