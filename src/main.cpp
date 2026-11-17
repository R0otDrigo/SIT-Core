#include <QApplication>
#include <QSettings>

#include "view/mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("TecnologiaObjetos");
    QCoreApplication::setApplicationName("appTecnologiaObjetos");

    // Credenciales de Firebase: config.ini junto al ejecutable (no se sube a git).
    QSettings config(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);

    MainWindow window;
    window.setApiConfig(config.value("FIREBASE_API_KEY").toString(),
                        config.value("FIREBASE_PROJECT_ID").toString());
    window.show();
    return app.exec();
}
