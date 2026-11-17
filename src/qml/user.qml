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
        zoomLevel: 36
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

    // Paleta compartida con el resto de la app (chrome oscuro sobre el mapa claro).
    readonly property color cardColor: "#1B2336"
    readonly property color borderColor: "#334155"
    readonly property color textColor: "#F8FAFC"
    readonly property color mutedColor: "#94A3B8"

    component SectionTitle: Text {
        width: parent ? parent.width : 0
        color: "#94A3B8"
        font.pixelSize: 11
        font.bold: true
        font.capitalization: Font.AllUppercase
        topPadding: 6
    }
    component Field: Row {
        property alias label: fieldLabel.text
        property alias value: fieldValue.text
        width: parent ? parent.width : 0
        spacing: 8
        Text { id: fieldLabel; width: 110; color: "#94A3B8"; font.pixelSize: 12 }
        Text {
            id: fieldValue
            width: parent.width - 118
            color: "#F8FAFC"
            font.pixelSize: 12
            wrapMode: Text.WordWrap
        }
    }

    Rectangle {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 44
        color: "#0F172A"

        Text {
            anchors.centerIn: parent
            text: qsTr("Mi espacio · vista de solo lectura")
            color: root.textColor
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
        height: statisticsColumn.implicitHeight + 24
        radius: 8
        color: root.cardColor
        border.color: root.borderColor

        Column {
            id: statisticsColumn
            anchors.fill: parent
            anchors.margins: 12
            spacing: 6

            Text {
                width: parent.width
                text: userBridge.displayName
                color: root.textColor
                font.bold: true
                font.pixelSize: 16
                elide: Text.ElideRight
            }
            Text {
                width: parent.width
                text: qsTr("Pasajeros %1 / %2").arg(userBridge.passengers).arg(userBridge.capacity)
                color: root.textColor
                font.pixelSize: 22
                font.bold: true
            }

            SectionTitle { text: qsTr("Servicio") }
            Field { label: qsTr("Bus asignado"); value: userBridge.vehicleId || "-" }
            Field { label: qsTr("Ruta"); value: userBridge.routeLabel || userBridge.routeId || "-" }
            Field { label: qsTr("Km recorridos"); value: userBridge.kilometers.toFixed(1) }
            Field { label: qsTr("Evento"); value: userBridge.evento || "-" }

            SectionTitle { text: qsTr("Vehículo") }
            Field { label: qsTr("Placa"); value: userBridge.placa || "-" }
            Field { label: qsTr("Marca"); value: userBridge.marca || "-" }

            SectionTitle { text: qsTr("Conductor") }
            Field { label: qsTr("Información"); value: userBridge.info || "-" }
            Field { label: qsTr("Antecedentes"); value: userBridge.antecedentes || "-" }

            SectionTitle { text: qsTr("Dispositivos") }
            Field { label: qsTr("Cámara"); value: qsTr("sin dispositivo") }
            Field { label: qsTr("Chat de voz"); value: qsTr("sin dispositivo") }
            Field { label: qsTr("Reportes"); value: "0" }

            Text {
                width: parent.width
                topPadding: 4
                text: qsTr("Usa el menú Sesión para ver más información, llamar o revisar el estado del vehículo.")
                font.pixelSize: 11
                color: root.mutedColor
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
        radius: 8
        color: root.cardColor
        border.color: root.borderColor

        Text {
            anchors.centerIn: parent
            text: userBridge.displayName
            color: root.mutedColor
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            width: parent.width - 12
        }
    }
}
