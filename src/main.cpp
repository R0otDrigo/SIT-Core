#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QUrl>

#include "utils/Logger.h"
#include "viewmodel/SimulationView.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("TEO-TO Simulator"));
    QGuiApplication::setOrganizationName(QStringLiteral("teo-to"));

    const QString logPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                            + QStringLiteral("/teo_to.log");
    teoto::installFileLogger(logPath);

    teoto::SimulationView simulation;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("sim"), &simulation);

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
