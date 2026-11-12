// Prueba de humo de controlador + vista (se ejecuta con QT_QPA_PLATFORM=offscreen).
#include <QApplication>
#include <QTimer>
#include "check.h"
#include "common/Exceptions.h"
#include "view/MainWindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    auto net = SimulationController::loadNetwork(QString(SIT_DATA_DIR) + "/network.csv");
    MainWindow window(std::move(net));
    window.show();

    // Dejamos correr el event loop: los snapshots del hilo de simulacion llegan encolados.
    int snapshots = 0;
    QObject::connect(window.controller(), &SimulationController::snapshotReady, [&](const sit::Snapshot&) { ++snapshots; });
    QTimer::singleShot(600, &app, &QApplication::quit);
    app.exec();

    CHECK(snapshots > 3);
    CHECK(window.stopsTable()->rowCount() == 15);

    // Un error del modelo llega a la vista como senal, sin romper nada.
    int errors = 0;
    QObject::connect(window.controller(), &SimulationController::errorOccurred, [&](const QString&) { ++errors; });
    window.controller()->dispatchBus("NOPE", 0);
    CHECK(errors == 1);

    // Archivo inexistente -> excepcion propia, no crash.
    bool threw = false;
    try { SimulationController::loadNetwork("/no/existe.csv"); } catch (const sit::SitException&) { threw = true; }
    CHECK(threw);
    return 0;
}
