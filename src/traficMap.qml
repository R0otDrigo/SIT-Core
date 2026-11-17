import QtQuick 2.15
import QtQuick.Controls
import QtLocation
import QtPositioning

Window {
    width: 800
    height: 600
    visible: true
    title: "OSM Map View"

    // 1. The Plugin tells QML to use OpenStreetMap as the data provider
    Plugin {
        id: mapPlugin
        name: "osm"
        PluginParameter { name: "osm.mapping.host"; value: "https://tile.openstreetmap.org/" }
        PluginParameter { name: "osm.geocoding.host"; value: "https://nominatim.openstreetmap.org" }
        PluginParameter { name: "osm.routing.host"; value: "https://router.project-osrm.org/viaroute" }
        PluginParameter { name: "osm.places.host"; value: "https://nominatim.openstreetmap.org/search" }
        PluginParameter { name: "osm.mapping.copyright"; value: "" }
        PluginParameter { name: "osm.mapping.highdpi_tiles"; value: true }

    }

    // 2. The Map component handles the visual rendering
    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin

        // Set the starting point (Latitude, Longitude)
        center: QtPositioning.coordinate(40.7128, -74.0060) // Example: New York
        zoomLevel: 12

        // Optional: Add a marker (MapQuickItem) to point to a specific spot
        MapQuickItem {
            coordinate: QtPositioning.coordinate(40.7128, -74.0060)
            anchorPoint.x: markerIcon.width / 2
            anchorPoint.y: markerIcon.height

            sourceItem: Rectangle {
                id: markerIcon
                width: 20; height: 20
                color: "red"
                radius: 10
                border.color: "white"
                border.width: 2
            }
        }
    }
}