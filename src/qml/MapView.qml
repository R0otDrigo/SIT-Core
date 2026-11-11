import QtQuick
import QtLocation
import QtPositioning

// Mapa base real (OpenStreetMap) con la ruta, los paraderos y el bus.
// Si el plugin no carga o no hay red, Main.qml conmuta a MapFallback.
Map {
    id: map

    plugin: Plugin { name: "osm" }
    center: QtPositioning.coordinate(-16.3989, -71.5369)
    zoomLevel: 13
    copyrightsVisible: true

    MapPolyline {
        line.width: 4
        line.color: "#1565c0"
        path: sim.routePath
    }

    MapItemView {
        model: sim.stops
        delegate: MapQuickItem {
            coordinate: QtPositioning.coordinate(modelData.lat, modelData.lon)
            anchorPoint.x: marker.width / 2
            anchorPoint.y: marker.height

            sourceItem: Column {
                id: marker
                spacing: 2

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 16
                    height: 16
                    radius: 8
                    color: "#2e7d32"
                    border.color: "white"
                    border.width: 2
                }
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    radius: 3
                    color: "#ffffffe6"
                    border.color: "#9e9e9e"
                    implicitWidth: stopLabel.implicitWidth + 8
                    implicitHeight: stopLabel.implicitHeight + 4

                    Text {
                        id: stopLabel
                        anchors.centerIn: parent
                        text: modelData.name
                        font.pixelSize: 11
                        color: "#222222"
                    }
                }
            }
        }
    }

    MapQuickItem {
        coordinate: sim.busPosition
        anchorPoint.x: busMarker.width / 2
        anchorPoint.y: busMarker.height / 2

        sourceItem: Rectangle {
            id: busMarker
            width: 30
            height: 18
            radius: 4
            color: "#e53935"
            border.color: "#7f0000"
            border.width: 2

            Text {
                anchors.centerIn: parent
                text: "BUS"
                color: "white"
                font.pixelSize: 9
                font.bold: true
            }
        }
    }

    onErrorChanged: {
        // En Qt 6.11 Map expone error/errorString en lugar de status.
        // QGeoServiceProvider::NoError == 0.
        if (error !== 0 && sim.mapMode !== "offline")
            Qt.callLater(function() { sim.mapMode = "offline" })
    }
}
