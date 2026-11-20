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
    property bool focused: false   // ruta elegida en el panel
    property bool dimmed: false    // hay otra ruta elegida

    MapPolyline {
        path: root.routePath.length > 1 ? root.routePath : root.routeWaypoints
        line.color: root.routeAffected ? "#EF4444" : root.routeColor
        line.width: (root.routeTrunk ? 6 : 4) + (root.focused ? 3 : 0)
        opacity: root.dimmed ? 0.15 : root.routeAffected ? 0.95 : 0.8
        z: root.focused ? 3 : 2
    }
}
