import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning

Item {
    id: root

    Plugin {
        id: osmPlugin
        name: "osm"
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
        center: QtPositioning.coordinate(userBridge.centerLatitude, userBridge.centerLongitude)
        zoomLevel: userBridge.initialZoom
        minimumZoomLevel: 9
        maximumZoomLevel: 18
        color: "#e8eef5"
        copyrightsVisible: true

        MapItemView {
            model: userBridge.vehicleModel
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
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 44
        color: "#0f172a"

        Text {
            anchors.centerIn: parent
            text: qsTr("Mi espacio - vista de solo lectura")
            color: "white"
            font.pixelSize: 16
            font.bold: true
        }
    }

    Rectangle {
        id: statistics
        anchors.right: parent.right
        anchors.top: header.bottom
        anchors.margins: 12
        width: 300
        radius: 8
        color: "#eeffffff"
        border.color: "#cbd5e1"

        Column {
            id: statisticsColumn
            anchors.fill: parent
            anchors.margins: 12
            spacing: 6

            Label {
                width: parent.width
                text: userBridge.displayName
                font.bold: true
                font.pixelSize: 15
            }
            Label {
                width: parent.width
                text: qsTr("Bus registrado a su nombre: %1").arg(userBridge.vehicleId || "-")
            }
            Label {
                width: parent.width
                text: qsTr("Kilometros recorridos: %1").arg(userBridge.kilometers.toFixed(1))
            }
            Label {
                width: parent.width
                text: qsTr("Ruta: %1").arg(userBridge.routeLabel || userBridge.routeId || "-")
            }
            Label {
                width: parent.width
                text: qsTr("Pasajeros: %1 / %2").arg(userBridge.passengers).arg(userBridge.capacity)
            }
            Label {
                width: parent.width
                text: qsTr("Informacion: %1").arg(userBridge.info)
                wrapMode: Text.WordWrap
            }
            Label {
                width: parent.width
                text: qsTr("Antecedentes: %1").arg(userBridge.antecedentes)
                wrapMode: Text.WordWrap
            }
            Label {
                width: parent.width
                text: qsTr("Evento: %1").arg(userBridge.evento)
                wrapMode: Text.WordWrap
            }
            Rectangle {
                width: parent.width
                height: 1
                color: "#cbd5e1"
            }
            Label {
                width: parent.width
                text: qsTr("Placa: %1").arg(userBridge.placa || "-")
            }
            Label {
                width: parent.width
                text: qsTr("Marca: %1").arg(userBridge.marca || "-")
            }
            Rectangle {
                width: parent.width
                height: 1
                color: "#cbd5e1"
            }
            Label {
                width: parent.width
                text: qsTr("Camara: sin dispositivo")
            }
            Label {
                width: parent.width
                text: qsTr("Chat de voz: sin dispositivo")
            }
            Label {
                width: parent.width
                text: qsTr("Reportes: 0")
            }
            Label {
                width: parent.width
                text: qsTr("Menu User: mas informacion, llamar y estado del vehiculo")
                font.pixelSize: 10
                color: "#64748b"
                wrapMode: Text.WordWrap
            }
        }
    }

    Rectangle {
        id: profilePhoto
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 12
        width: 120
        height: 140
        radius: 6
        color: "#eeffffff"
        border.color: "#cbd5e1"

        Text {
            anchors.centerIn: parent
            text: userBridge.displayName
            color: "#64748b"
            font.pixelSize: 11
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            width: parent.width - 12
        }
    }
}
