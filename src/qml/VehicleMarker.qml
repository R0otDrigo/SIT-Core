import QtQuick
import QtLocation

MapQuickItem {
    id: root

    property color routeColor: "#6b7280"
    property int passengers: 0
    property int capacity: 0
    property int vehicleId: 0
    property string routeId: ""
    property string busType: ""
    property bool dwelling: false

    anchorPoint.x: bus.width / 2
    anchorPoint.y: bus.height / 2

    sourceItem: Rectangle {
        id: bus
        width: 26 + Math.min(20, Math.floor(root.capacity / 8))
        height: 15
        radius: 4
        color: root.routeColor
        border.color: "#0f172a"
        border.width: 1
        opacity: root.dwelling ? 0.72 : 1.0

        Text {
            anchors.centerIn: parent
            text: root.passengers
            color: "white"
            font.pixelSize: 8
            font.bold: true
        }
    }
}
