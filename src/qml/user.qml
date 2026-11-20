import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning
import "Theme.js" as Theme

Item {
    id: root

    readonly property int occupancyPercent: userBridge.capacity > 0
        ? Math.round(100 * userBridge.passengers / userBridge.capacity) : 0
    readonly property real occupancyRatio: userBridge.capacity > 0
        ? Math.min(1, userBridge.passengers / userBridge.capacity) : 0
    readonly property color occupancyColor: Theme.occupancyColor(occupancyRatio)

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
        color: Theme.mapBg
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

    // Tarjeta del vehículo asignado
    Rectangle {
        id: vehicleCard
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 16
        width: 330
        height: vehicleColumn.implicitHeight + 32
        radius: Theme.radius
        color: Theme.surface
        border.color: Theme.border
        border.width: 1

        Column {
            id: vehicleColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 16
            spacing: 10

            Row {
                width: parent.width
                spacing: 8
                Text {
                    text: qsTr("Mi vehículo")
                    color: Theme.muted
                    font.pixelSize: 11
                    font.bold: true
                }
                Item { width: parent.width - 150; height: 1 }
                Rectangle {
                    width: routeText.implicitWidth + 16
                    height: 24
                    radius: 12
                    color: "#EFF6FF"
                    border.color: "#BFDBFE"
                    Text {
                        id: routeText
                        anchors.centerIn: parent
                        text: userBridge.routeId || "—"
                        color: Theme.primaryDark
                        font.pixelSize: 10
                        font.bold: true
                    }
                }
            }

            Row {
                spacing: 10
                Text {
                    text: "#"
                    color: Theme.muted
                    font.pixelSize: 22
                    font.bold: true
                    anchors.bottom: vehicleIdText.bottom
                }
                Text {
                    id: vehicleIdText
                    text: userBridge.vehicleId || "—"
                    color: Theme.text
                    font.pixelSize: 34
                    font.bold: true
                }
            }

            Text {
                width: parent.width
                text: userBridge.routeLabel || qsTr("Ruta sin asignar")
                color: Theme.text
                font.pixelSize: 14
                font.weight: Font.DemiBold
                elide: Text.ElideRight
            }

            Row {
                spacing: 18
                Column {
                    spacing: 1
                    Text { text: qsTr("Tipo"); color: Theme.muted; font.pixelSize: 10 }
                    Text { text: userBridge.vehicleType || "—"; color: Theme.text; font.pixelSize: 13; font.bold: true }
                }
                Column {
                    spacing: 1
                    Text { text: qsTr("Placa"); color: Theme.muted; font.pixelSize: 10 }
                    Text { text: userBridge.placa || "—"; color: Theme.text; font.pixelSize: 13; font.bold: true }
                }
                Column {
                    spacing: 1
                    Text { text: qsTr("Marca"); color: Theme.muted; font.pixelSize: 10 }
                    Text { text: userBridge.marca || "—"; color: Theme.text; font.pixelSize: 13; font.bold: true }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: Theme.border
            }

            Row {
                width: parent.width
                Text {
                    text: qsTr("Ocupación")
                    color: Theme.muted
                    font.pixelSize: 11
                    font.bold: true
                }
                Item { width: parent.width - occupancyText.implicitWidth - 80; height: 1 }
                Text {
                    id: occupancyText
                    text: root.occupancyPercent + "%"
                    color: root.occupancyColor
                    font.pixelSize: 13
                    font.bold: true
                }
            }

            Rectangle {
                width: parent.width
                height: 9
                radius: 5
                color: "#E2E8F0"
                Rectangle {
                    width: parent.width * root.occupancyRatio
                    height: parent.height
                    radius: 5
                    color: root.occupancyColor
                }
            }

            Row {
                width: parent.width
                Text {
                    text: qsTr("%1 / %2 pasajeros")
                          .arg(userBridge.passengers).arg(userBridge.capacity)
                    color: Theme.muted
                    font.pixelSize: 11
                }
                Item { width: parent.width - kmText.implicitWidth - 140; height: 1 }
                Text {
                    id: kmText
                    text: qsTr("%1 km").arg(userBridge.kilometers.toFixed(1))
                    color: Theme.muted
                    font.pixelSize: 11
                }
            }

            Rectangle {
                width: parent.width
                height: 28
                radius: 8
                color: userBridge.dwelling ? "#FEF3C7" : "#DCFCE7"
                Text {
                    anchors.centerIn: parent
                    text: userBridge.dwelling ? qsTr("Detenido en paradero")
                                              : qsTr("En ruta")
                    color: userBridge.dwelling ? "#92400E" : "#166534"
                    font.pixelSize: 11
                    font.bold: true
                }
            }
        }
    }

    // Tarjeta de información y novedades
    Rectangle {
        id: infoCard
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 16
        width: 310
        height: infoColumn.implicitHeight + 32
        visible: root.width > 1120
        radius: Theme.radius
        color: Theme.surface
        border.color: Theme.border
        border.width: 1

        Column {
            id: infoColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 16
            spacing: 10

            Text {
                text: qsTr("Estado del servicio")
                color: Theme.muted
                font.pixelSize: 11
                font.bold: true
            }
            Text {
                width: parent.width
                text: userBridge.info || qsTr("Sin novedades")
                color: Theme.text
                font.pixelSize: 12
                wrapMode: Text.WordWrap
            }

            Rectangle { width: parent.width; height: 1; color: Theme.border }

            Text { text: qsTr("Antecedentes"); color: Theme.muted; font.pixelSize: 10; font.bold: true }
            Text {
                width: parent.width
                text: userBridge.antecedentes || qsTr("Sin antecedentes registrados")
                color: Theme.text
                font.pixelSize: 12
                wrapMode: Text.WordWrap
            }

            Text { text: qsTr("Evento"); color: Theme.muted; font.pixelSize: 10; font.bold: true }
            Text {
                width: parent.width
                text: userBridge.evento || qsTr("Sin eventos activos")
                color: Theme.text
                font.pixelSize: 12
                wrapMode: Text.WordWrap
            }

            Rectangle { width: parent.width; height: 1; color: Theme.border }

            Text { text: qsTr("Próximamente"); color: Theme.muted; font.pixelSize: 10; font.bold: true }
            Text {
                width: parent.width
                text: qsTr("Cámara, chat de voz y reportes estarán disponibles en una próxima versión.")
                color: Theme.muted
                font.pixelSize: 11
                wrapMode: Text.WordWrap
            }
        }
    }

    // Estado sin vehículo
    Rectangle {
        visible: !userBridge.hasVehicle
        anchors.centerIn: parent
        width: 320
        height: emptyColumn.implicitHeight + 40
        radius: Theme.radius
        color: Theme.surface
        border.color: Theme.border
        border.width: 1

        Column {
            id: emptyColumn
            anchors.centerIn: parent
            width: parent.width - 40
            spacing: 8
            Text {
                width: parent.width
                text: qsTr("Sin vehículo asignado")
                color: Theme.text
                font.pixelSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                width: parent.width
                text: qsTr("El administrador todavía no te asignó un bus. Esta vista se actualizará automáticamente.")
                color: Theme.muted
                font.pixelSize: 12
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
