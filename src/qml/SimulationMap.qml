import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning
import "Theme.js" as Theme

Item {
    id: root

    property bool legendVisible: true

    component MapButton: Button {
        id: control
        width: 40
        height: 40
        contentItem: Text {
            text: control.text
            color: Theme.text
            font.pixelSize: 18
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            radius: 9
            color: control.down ? "#E2E8F0" : Theme.surface
            border.color: Theme.border
            border.width: 1
        }
    }

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
        color: Theme.mapBg
        copyrightsVisible: true

        DragHandler {
            target: null
            onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
        }

        WheelHandler {
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

    // Banner de evento activo
    Rectangle {
        id: eventBanner
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: mapBridge.eventText.length > 0 ? 40 : 0
        visible: height > 0
        color: Theme.danger

        Text {
            anchors.centerIn: parent
            width: parent.width - 24
            text: mapBridge.eventText
            color: "white"
            font.pixelSize: 12
            font.bold: true
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
        }
    }

    // Leyenda de rutas
    Rectangle {
        id: legend
        anchors.left: parent.left
        anchors.top: eventBanner.bottom
        anchors.margins: 14
        width: 230
        height: legendColumn.implicitHeight + 24
        visible: root.legendVisible && routeRepeater.count > 0
        radius: Theme.radius
        color: Theme.surface
        border.color: Theme.border
        border.width: 1

        Column {
            id: legendColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 12
            spacing: 7

            Row {
                width: parent.width
                Text {
                    text: qsTr("Rutas")
                    color: Theme.text
                    font.pixelSize: 12
                    font.bold: true
                }
                Item { width: parent.width - 70; height: 1 }
                Text {
                    text: qsTr("Ocultar")
                    color: Theme.primary
                    font.pixelSize: 10
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.legendVisible = false
                    }
                }
            }

            Repeater {
                id: routeRepeater
                model: mapBridge.routeModel
                delegate: Row {
                    width: legendColumn.width
                    spacing: 6
                    Rectangle {
                        width: 14
                        height: 4
                        radius: 2
                        color: model.color
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        width: parent.width - 22
                        text: model.name
                        color: Theme.text
                        font.pixelSize: 10
                        elide: Text.ElideRight
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: Theme.border
            }

            Text {
                text: qsTr("Paraderos")
                color: Theme.muted
                font.pixelSize: 10
                font.bold: true
            }
            Row {
                spacing: 6
                Rectangle { width: 9; height: 9; radius: 5; color: Theme.success; anchors.verticalCenter: parent.verticalCenter }
                Text { text: qsTr("Baja"); color: Theme.muted; font.pixelSize: 10 }
                Rectangle { width: 9; height: 9; radius: 5; color: Theme.warning; anchors.verticalCenter: parent.verticalCenter }
                Text { text: qsTr("Media"); color: Theme.muted; font.pixelSize: 10 }
                Rectangle { width: 9; height: 9; radius: 5; color: Theme.danger; anchors.verticalCenter: parent.verticalCenter }
                Text { text: qsTr("Alta"); color: Theme.muted; font.pixelSize: 10 }
            }
        }
    }

    // Boton para recuperar la leyenda
    MapButton {
        visible: !root.legendVisible
        anchors.left: parent.left
        anchors.top: eventBanner.bottom
        anchors.margins: 14
        text: qsTr("≡")
        onClicked: root.legendVisible = true
    }

    // Controles de zoom
    Column {
        anchors.right: parent.right
        anchors.top: eventBanner.bottom
        anchors.margins: 14
        spacing: 8

        MapButton {
            text: "+"
            onClicked: map.zoomLevel = Math.min(map.maximumZoomLevel, map.zoomLevel + 1)
        }
        MapButton {
            text: "−"
            onClicked: map.zoomLevel = Math.max(map.minimumZoomLevel, map.zoomLevel - 1)
        }
        MapButton {
            text: "⌂"
            onClicked: {
                map.center = QtPositioning.coordinate(mapBridge.centerLatitude,
                                                     mapBridge.centerLongitude)
                map.zoomLevel = mapBridge.initialZoom
            }
        }
    }

    // Estado del mapa
    Rectangle {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 14
        width: statusColumn.width + 24
        height: statusColumn.height + 16
        radius: Theme.radiusSmall
        color: Theme.surface
        border.color: Theme.border
        border.width: 1

        Column {
            id: statusColumn
            anchors.centerIn: parent
            spacing: 4

            Text {
                text: mapBridge.gtfsStatus.length > 0 ? mapBridge.gtfsStatus
                                                      : qsTr("GTFS no cargado")
                color: Theme.muted
                font.pixelSize: 10
            }
            Row {
                spacing: 8
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Trazados GTFS")
                    color: Theme.text
                    font.pixelSize: 11
                }
                Switch {
                    checked: mapBridge.showGtfs
                    onToggled: mapBridge.showGtfs = checked
                }
            }
        }
    }
}
