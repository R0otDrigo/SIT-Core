#include <QApplication>
#include <QSettings>

#include "view/apptheme.h"
#include "view/mainwindow.h"

int main(int argc, char *argv[]) {
    // QQuickWidget comparte contextos OpenGL con la ventana principal.
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("TecnologiaObjetos");
    QCoreApplication::setApplicationName("appTecnologiaObjetos");

    applyAppTheme(a);

    // Credenciales de Firebase: config.ini junto al ejecutable (no se sube a git).
    QSettings config(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);

    QString apiKey = config.value("FIREBASE_API_KEY").toString();
    QString projectId = config.value("FIREBASE_PROJECT_ID").toString();

    MainWindow w;
    w.setApiConfig(apiKey, projectId);
    w.show();

    return a.exec();
}
