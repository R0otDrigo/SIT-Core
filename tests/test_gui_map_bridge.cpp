#include <QApplication>
#include "check.h"
#include "controller/SimulationController.h"
#include "view/MapBridge.h"
#include "view/MapModels.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    auto net = SimulationController::loadNetwork(QString(SIT_DATA_DIR) + "/network.csv");
    // Con QApplication activo el locale global puede ser es_PE (coma decimal):
    // las coordenadas deben seguir parseandose correctamente.
    CHECK_NEAR(net->stops().byId("CAY")->geoPosition().latitude, -16.37, 1e-6);
    CHECK_NEAR(net->stops().byId("CAY")->geoPosition().longitude, -71.545, 1e-6);
    MapBridge bridge;
    bridge.update(net->snapshot());

    CHECK(bridge.routeModel()->rowCount() == 4);
    CHECK(bridge.stopModel()->rowCount() == 15);
    CHECK(bridge.vehicleModel()->rowCount() == 6);

    const QModelIndex routeIndex = bridge.routeModel()->index(0, 0);
    const QVariantList waypoints =
        bridge.routeModel()->data(routeIndex, RouteListModel::WaypointsRole).toList();
    CHECK(waypoints.size() >= 2);
    CHECK(!bridge.routeModel()->data(routeIndex, RouteListModel::ColorRole).toString().isEmpty());

    const QModelIndex stopIndex = bridge.stopModel()->index(0, 0);
    const double lat = bridge.stopModel()->data(stopIndex, StopListModel::LatitudeRole).toDouble();
    const double lon = bridge.stopModel()->data(stopIndex, StopListModel::LongitudeRole).toDouble();
    CHECK(lat < -16.0 && lat > -17.0);
    CHECK(lon < -71.0 && lon > -72.0);
    CHECK(bridge.centerLatitude() < -16.0 && bridge.centerLatitude() > -17.0);

    // Un evento de trafico marca la ruta afectada en el modelo QML.
    net->triggerEvent(sit::EventType::Accident, "T1");
    bridge.update(net->snapshot());
    bool affected = false;
    for (int i = 0; i < bridge.routeModel()->rowCount(); ++i) {
        const QModelIndex idx = bridge.routeModel()->index(i, 0);
        if (bridge.routeModel()->data(idx, RouteListModel::RouteIdRole).toString() == "T1")
            affected = bridge.routeModel()->data(idx, RouteListModel::AffectedRole).toBool();
    }
    CHECK(affected);
    CHECK(!bridge.eventText().isEmpty());
    return 0;
}
