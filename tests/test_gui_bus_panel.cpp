#include <QApplication>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include "check.h"
#include "view/SimulationWidget.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    auto network = SimulationController::loadNetwork(QString(SIT_DATA_DIR) + "/network.csv");
    SimulationWidget window(std::move(network));
    window.controller()->setPaused(true);
    window.show();

    auto* map = window.findChild<MapView*>();
    auto* panel = window.findChild<QGroupBox*>("busInfoPanel");
    auto* details = window.findChild<QWidget*>("busInfoDetails");
    auto* passengers = window.findChild<QLabel*>("busPassengersLabel");
    auto* freeSeats = window.findChild<QLabel*>("busFreeSeatsLabel");
    auto* occupancy = window.findChild<QProgressBar*>("busOccupancyBar");
    CHECK(map && panel && details && passengers && freeSeats && occupancy);
    CHECK(details->isHidden());

    map->busClicked(7, "Articulado", "T1 - Troncal", 23, 160, true);
    CHECK(panel->title() == "Bus #7");
    CHECK(!details->isHidden());
    CHECK(passengers->text() == "23 / 160");
    CHECK(freeSeats->text() == "137");
    CHECK(occupancy->value() == 14);

    sit::Snapshot snapshot;
    snapshot.clock = "08:00";
    sit::VehicleView bus;
    bus.id = 7;
    bus.type = "Articulado";
    bus.routeId = "T1";
    bus.passengers = 80;
    bus.capacity = 160;
    snapshot.vehicles.push_back(bus);
    sit::RouteView route;
    route.id = "T1";
    route.name = "Troncal";
    snapshot.routes.push_back(route);
    CHECK(QMetaObject::invokeMethod(&window, "onSnapshot", Qt::DirectConnection,
                                    Q_ARG(sit::Snapshot, snapshot)));
    CHECK(passengers->text() == "80 / 160");
    CHECK(freeSeats->text() == "80");
    CHECK(occupancy->value() == 50);

    snapshot.vehicles.clear();
    CHECK(QMetaObject::invokeMethod(&window, "onSnapshot", Qt::DirectConnection,
                                    Q_ARG(sit::Snapshot, snapshot)));
    CHECK(details->isHidden());
    CHECK(panel->title() == "Información del bus");
    return 0;
}
