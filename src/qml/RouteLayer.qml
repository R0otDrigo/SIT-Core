import QtQuick
import QtLocation

MapItemGroup {
    id: root

    property var routeWaypoints: []
    property var routePath: []
    property color routeColor: "#1d70b8"
    property bool routeTrunk: true
    property bool routeAffected: false
    property string routeLabel: ""

    MapPolyline {
        path: root.routePath.length > 1 ? root.routePath : root.routeWaypoints
        line.color: root.routeAffected ? "#dc2626" : root.routeColor
        line.width: root.routeTrunk ? 6 : 4
        opacity: root.routeAffected ? 0.95 : 0.8
        z: 2
    }
}
