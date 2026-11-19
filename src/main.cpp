#include <QApplication>
#include <QFile>
#include <QSettings>

#include "controller/firebaseauthmanager.h"
#include "controller/UnitRegistry.h"
#include "view/mainwindow.h"

int main(int argc, char *argv[]) {
    // QQuickWidget comparte contextos OpenGL con la ventana principal.
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("TecnologiaObjetos");
    QCoreApplication::setApplicationName("appTecnologiaObjetos");

    // Tema oscuro global (src/view/theme.qss).
    QApplication::setStyle("Fusion");
    QFile theme(":/style/theme.qss");
    if (theme.open(QIODevice::ReadOnly | QIODevice::Text))
        a.setStyleSheet(QString::fromUtf8(theme.readAll()));

    // Credenciales de Firebase: config.ini junto al ejecutable (no se sube a git).
    QSettings config(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    QString apiKey = config.value("FIREBASE_API_KEY").toString();
    QString projectId = config.value("FIREBASE_PROJECT_ID").toString();

    FirebaseAuthManager::instance().setApiKey(apiKey);
    FirebaseAuthManager::instance().setProjectId(projectId);
    UnitRegistry::instance().configure(projectId, QString());

    MainWindow w;
    w.setApiConfig(apiKey, projectId);
    w.show();

    return a.exec();
}
