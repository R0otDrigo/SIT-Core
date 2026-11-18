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
    CHECK_NEAR(net->stops().byId("10270810751")->geoPosition().latitude, -16.4044512, 1e-6);
    CHECK_NEAR(net->stops().byId("10270810751")->geoPosition().longitude, -71.4896626, 1e-6);
    MapBridge bridge;
    bridge.update(net->snapshot());

    CHECK(bridge.routeModel()->rowCount() == 9);
    CHECK(bridge.stopModel()->rowCount() == 288);
    CHECK(bridge.vehicleModel()->rowCount() == 12);

    const QModelIndex routeIndex = bridge.routeModel()->index(0, 0);
    const QVariantList waypoints =
        bridge.routeModel()->data(routeIndex, RouteListModel::WaypointsRole).toList();
    CHECK(waypoints.size() >= 2);
    const QVariantList path =
        bridge.routeModel()->data(routeIndex, RouteListModel::PathRole).toList();
    CHECK(path.size() >= 2);
    CHECK(!bridge.routeModel()->data(routeIndex, RouteListModel::ColorRole).toString().isEmpty());

    const QModelIndex stopIndex = bridge.stopModel()->index(0, 0);
    const double lat = bridge.stopModel()->data(stopIndex, StopListModel::LatitudeRole).toDouble();
    const double lon = bridge.stopModel()->data(stopIndex, StopListModel::LongitudeRole).toDouble();
    CHECK(lat < -16.0 && lat > -17.0);
    CHECK(lon < -71.0 && lon > -72.0);
    CHECK(bridge.centerLatitude() < -16.0 && bridge.centerLatitude() > -17.0);

    // Un evento de trafico marca la ruta afectada en el modelo QML.
    net->triggerEvent(sit::EventType::Accident, "17643070");
    bridge.update(net->snapshot());
    bool affected = false;
    for (int i = 0; i < bridge.routeModel()->rowCount(); ++i) {
        const QModelIndex idx = bridge.routeModel()->index(i, 0);
        if (bridge.routeModel()->data(idx, RouteListModel::RouteIdRole).toString() == "17643070")
            affected = bridge.routeModel()->data(idx, RouteListModel::AffectedRole).toBool();
    }
    CHECK(affected);
    CHECK(!bridge.eventText().isEmpty());
    return 0;
}
