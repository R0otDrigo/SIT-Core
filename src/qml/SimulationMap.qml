import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning

Item {
    id: root

    Plugin {
        id: osmPlugin
        name: "osm"
        // Usar HTTPS directo y evitar la consulta a maps-redirect.qt.io, que en
        // algunas redes devuelve HTML y deja el mapa sin tiles.
        PluginParameter { name: "osm.mapping.providersrepository.disabled"; value: true }
        PluginParameter { name: "osm.mapping.custom.host"; value: "https://tile.openstreetmap.org/" }
        PluginParameter { name: "osm.mapping.copyright"; value: "© OpenStreetMap contributors" }
        PluginParameter { name: "osm.routing.host"; value: "https://router.project-osrm.org/route/v1/driving/" }
        PluginParameter { name: "osm.useragent"; value: "SIT-Core Arequipa" }
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: osmPlugin
        center: QtPositioning.coordinate(mapBridge.centerLatitude, mapBridge.centerLongitude)
        zoomLevel: mapBridge.initialZoom
        minimumZoomLevel: 9
        maximumZoomLevel: 18
        color: "#e8eef5"
        copyrightsVisible: true

        function selectCustomMapType() {
            for (let i = 0; i < supportedMapTypes.length; ++i) {
                if (supportedMapTypes[i].style === MapType.CustomMap) {
                    activeMapType = supportedMapTypes[i];
                    return;
                }
            }
        }
        Component.onCompleted: selectCustomMapType()

        // Trazados reales importados de GTFS (routes/trips/shapes).
        MapItemView {
            model: mapBridge.gtfsRouteModel
            delegate: MapPolyline {
                path: model.shape
                line.color: model.color
                line.width: 2
                opacity: mapBridge.showGtfs ? 0.30 : 0.0
                visible: mapBridge.showGtfs
                z: 0
            }
        }

        // Rutas de la simulacion: cada una tiene su propio RouteQuery + RouteModel.
        MapItemView {
            model: mapBridge.routeModel
            delegate: RouteLayer {
                mapPlugin: osmPlugin
                routeWaypoints: model.waypoints
                routeColor: model.color
                routeTrunk: model.trunk
                routeAffected: model.affected
                routeLabel: model.name
            }
        }

        MapItemView {
            model: mapBridge.stopModel
            delegate: StopMarker {
                coordinate: QtPositioning.coordinate(model.latitude, model.longitude)
                stopName: model.name
                waiting: model.waiting
                saturation: model.saturation
                closed: model.closed
            }
        }

        MapItemView {
            model: mapBridge.vehicleModel
            delegate: VehicleMarker {
                coordinate: QtPositioning.coordinate(model.latitude, model.longitude)
                routeColor: model.color
                passengers: model.passengers
                capacity: model.capacity
                vehicleId: model.vehicleId
                routeId: model.routeId
                busType: model.type
                dwelling: model.dwelling
            }
        }
    }

    Rectangle {
        id: eventBanner
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: mapBridge.eventText.length > 0 ? 36 : 0
        visible: height > 0
        color: "#dc2626"

        Text {
            anchors.centerIn: parent
            width: parent.width - 24
            text: mapBridge.eventText
            color: "white"
            font.bold: true
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
        }
    }

    Column {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 12
        spacing: 6

        Button {
            width: 38
            height: 32
            text: "+"
            onClicked: map.zoomLevel = Math.min(map.maximumZoomLevel, map.zoomLevel + 1)
        }
        Button {
            width: 38
            height: 32
            text: "\u2212"
            onClicked: map.zoomLevel = Math.max(map.minimumZoomLevel, map.zoomLevel - 1)
        }
        Button {
            width: 38
            height: 32
            text: "\u2302"
            onClicked: {
                map.center = QtPositioning.coordinate(mapBridge.centerLatitude,
                                                     mapBridge.centerLongitude)
                map.zoomLevel = mapBridge.initialZoom
            }
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 12
        width: statusColumn.width + 20
        height: statusColumn.height + 12
        radius: 6
        color: "#eeffffff"
        border.color: "#cbd5e1"

        Column {
            id: statusColumn
            anchors.centerIn: parent
            spacing: 2

            Text {
                text: mapBridge.gtfsStatus.length > 0 ? mapBridge.gtfsStatus
                                                      : "GTFS no cargado"
                color: "#334155"
                font.pixelSize: 10
            }
            Row {
                spacing: 8
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Trazados GTFS"
                    color: "#334155"
                    font.pixelSize: 10
                }
                Switch {
                    checked: mapBridge.showGtfs
                    onToggled: mapBridge.showGtfs = checked
                }
            }
        }
    }
}
