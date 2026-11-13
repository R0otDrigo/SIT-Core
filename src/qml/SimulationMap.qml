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

        DragHandler {
            target: null
            onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
        }

        WheelHandler {
            // Rueda del raton y gestos del touchpad: un nivel de zoom por paso.
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            onWheel: (event) => {
                if (event.angleDelta.y > 0)
                    map.zoomLevel = Math.min(map.maximumZoomLevel, map.zoomLevel + 1)
                else
                    map.zoomLevel = Math.max(map.minimumZoomLevel, map.zoomLevel - 1)
                event.accepted = true
            }
        }

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

        // Rutas de la simulacion: el bus y la polilinea usan el mismo PATH del CSV/GTFS.
        MapItemView {
            model: mapBridge.routeModel
            delegate: RouteLayer {
                routeWaypoints: model.waypoints
                routePath: model.path
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
                showLabel: map.zoomLevel >= 15
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
                showStats: mapBridge.selectedVehicleId === String(model.vehicleId)
                statsText: mapBridge.selectedVehicleStats
                incomingCall: mapBridge.incomingCallVehicleId === String(model.vehicleId)
                callName: mapBridge.incomingCallName
                callStatus: mapBridge.incomingCallStatus
                onClicked: mapBridge.selectVehicle(String(model.vehicleId))
                onCallRequested: mapBridge.callRequested(String(model.vehicleId))
                onCallAnswered: mapBridge.callAnswered(String(model.vehicleId))
            }
        }
    }

    // Paleta compartida con el panel Qt (chrome oscuro sobre el mapa claro).
    readonly property color cardColor: "#1B2336"
    readonly property color borderColor: "#334155"
    readonly property color textColor: "#F8FAFC"
    readonly property color mutedColor: "#94A3B8"

    component MapButton: Button {
        id: mapButton
        width: 36
        height: 36
        font.pixelSize: 18
        contentItem: Text {
            text: mapButton.text
            font: mapButton.font
            color: "#F8FAFC"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            radius: 6
            color: mapButton.down ? "#272F42" : "#1B2336"
            border.color: "#334155"
        }
        ToolTip.visible: hovered && ToolTip.text.length > 0
        ToolTip.delay: 500
    }

    Rectangle {
        id: eventBanner
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: mapBridge.eventText.length > 0 ? 36 : 0
        visible: height > 0
        color: "#EF4444"

        Text {
            anchors.centerIn: parent
            width: parent.width - 24
            text: "\u26A0 " + mapBridge.eventText
            color: "white"
            font.bold: true
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
        }
    }

    Column {
        anchors.right: parent.right
        anchors.top: eventBanner.bottom
        anchors.margins: 12
        spacing: 6

        MapButton {
            text: "+"
            ToolTip.text: qsTr("Acercar")
            onClicked: map.zoomLevel = Math.min(map.maximumZoomLevel, map.zoomLevel + 1)
        }
        MapButton {
            text: "\u2212"
            ToolTip.text: qsTr("Alejar")
            onClicked: map.zoomLevel = Math.max(map.minimumZoomLevel, map.zoomLevel - 1)
        }
        MapButton {
            text: "\u2302"
            ToolTip.text: qsTr("Volver a la vista de Arequipa")
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
        radius: 8
        color: root.cardColor
        border.color: root.borderColor

        Column {
            id: statusColumn
            anchors.centerIn: parent
            spacing: 2

            Text {
                text: mapBridge.gtfsStatus.length > 0 ? mapBridge.gtfsStatus
                                                      : qsTr("GTFS no cargado")
                color: root.mutedColor
                font.pixelSize: 11
            }
            Row {
                spacing: 8
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Mostrar trazados GTFS")
                    color: root.textColor
                    font.pixelSize: 12
                }
                Switch {
                    checked: mapBridge.showGtfs
                    onToggled: mapBridge.showGtfs = checked
                }
            }
        }
    }
}
