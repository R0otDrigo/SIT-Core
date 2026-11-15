#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QMessageBox>
#include <QTimer>
#include "common/Exceptions.h"
#include "view/MainWindow.h"

#ifndef SIT_DATA_DIR
#define SIT_DATA_DIR "data"
#endif

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("SIT-Core");

    QCommandLineParser cli;
    cli.setApplicationDescription("Simulador y gestor de flota de transporte publico urbano");
    cli.addHelpOption();
    cli.addPositionalArgument("red", "Archivo de red (.csv). Por defecto data/network.csv", "[red]");
    QCommandLineOption shotOpt("screenshot", "Guarda una captura de la ventana tras unos segundos y sale (para pruebas).", "archivo");
    cli.addOption(shotOpt);
    cli.process(app);

    QString path;
    if (!cli.positionalArguments().isEmpty()) path = cli.positionalArguments().first();
    else if (QFile::exists(QDir(QApplication::applicationDirPath()).filePath("data/network.csv")))
        path = QDir(QApplication::applicationDirPath()).filePath("data/network.csv");
    else path = QString(SIT_DATA_DIR) + "/network.csv";

    std::unique_ptr<sit::Network> network;
    try {
        network = SimulationController::loadNetwork(path);
    } catch (const sit::SitException& e) {
        QMessageBox::critical(nullptr, "SIT-Core", QString("No se pudo cargar la red:\n%1").arg(e.what()));
        return 1;
    }

    MainWindow window(std::move(network));
    window.show();

    if (cli.isSet(shotOpt)) {                   // modo de pruebas: captura automatica
        const QString out = cli.value(shotOpt);
        window.controller()->setSpeed(10.0);
        QTimer::singleShot(1500, [&] { window.controller()->dispatchBus("T1", 0); });
        QTimer::singleShot(2500, [&] { window.controller()->dispatchBus("T2", 1); });
        QTimer::singleShot(5000, [&] { window.controller()->triggerEvent(0, "T1"); });
        QTimer::singleShot(8000, [&] { window.grab().save(out); QApplication::quit(); });
    }
    return app.exec();
}
