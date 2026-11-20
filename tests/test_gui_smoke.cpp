// Prueba de humo de controlador + vista (se ejecuta con QT_QPA_PLATFORM=offscreen).
#include <QApplication>
#include <QListWidget>
#include <QQuickWidget>
#include <QTimer>
#include "check.h"
#include "common/Exceptions.h"
#include "view/MapBridge.h"
#include "view/SimulationWidget.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    auto net = SimulationController::loadNetwork(QString(SIT_DATA_DIR) + "/network.csv");
    SimulationWidget window(std::move(net));
    window.show();

    // Dejamos correr el event loop: los snapshots del hilo de simulacion llegan encolados.
    int snapshots = 0;
    QObject::connect(window.controller(), &SimulationController::snapshotReady, [&](const sit::Snapshot&) { ++snapshots; });
    QTimer::singleShot(600, &app, &QApplication::quit);
    app.exec();

    CHECK(snapshots > 3);
    CHECK(window.stopsTable()->rowCount() == 288);
    CHECK(window.mapWidget() != nullptr);
    CHECK(window.mapWidget()->status() == QQuickWidget::Ready);
    CHECK(window.mapWidget()->errors().isEmpty());

    // Rutas: la lista tiene las 9; buscar oculta las que no coinciden.
    QListWidget* routes = window.routeList();
    CHECK(routes->count() == 9);
    window.findChild<QLineEdit*>("routeSearch")->setText("yumina");
    int visible = 0;
    for (int i = 0; i < routes->count(); ++i) visible += routes->item(i)->isHidden() ? 0 : 1;
    CHECK(visible == 1);

    // Elegir una ruta la resalta en el mapa y filtra la tabla a sus paraderos.
    window.focusRoute("21240658");
    CHECK(window.mapBridge()->focusedRouteId() == "21240658");
    const int routeStops = window.mapBridge()->focusedStops().size();
    CHECK(routeStops > 1 && routeStops < 288);
    QTimer::singleShot(200, &app, &QApplication::quit);
    app.exec();
    CHECK(window.stopsTable()->rowCount() == routeStops);
    window.focusRoute(QString());
    CHECK(window.mapBridge()->focusedStops().isEmpty());

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
