import QtQuick
import QtLocation
import QtPositioning

MapItemGroup {
    id: root

    property var mapPlugin: null
    property var routeWaypoints: []
    property color routeColor: "#1d70b8"
    property bool routeTrunk: true
    property bool routeAffected: false
    property string routeLabel: ""

    function rebuildQuery() {
        routeQuery.clearWaypoints()
        var points = root.routeWaypoints || []
        for (var i = 0; i < points.length; ++i) {
            var point = points[i]
            if (point)
                routeQuery.addWaypoint(QtPositioning.coordinate(point.latitude, point.longitude))
        }
        if (points.length >= 2 && root.mapPlugin)
            routeModel.update()
    }

    Component.onCompleted: rebuildQuery()
    onRouteWaypointsChanged: rebuildQuery()
    onMapPluginChanged: if (routeWaypoints.length >= 2) rebuildQuery()

    RouteQuery {
        id: routeQuery
        travelModes: RouteQuery.CarTravel
        routeOptimizations: RouteQuery.FastestRoute
        numberAlternativeRoutes: 1
    }

    RouteModel {
        id: routeModel
        plugin: root.mapPlugin
        query: routeQuery
        autoUpdate: false
    }

    // Respaldo offline: polilinea directa por los paraderos si OSRM no responde.
    MapPolyline {
        path: root.routeWaypoints
        visible: !(routeModel.status === RouteModel.Ready && routeModel.count > 0)
        line.color: root.routeAffected ? "#dc2626" : root.routeColor
        line.width: root.routeTrunk ? 6 : 4
        opacity: root.routeAffected ? 0.95 : 0.75
        z: 2
    }

    // Trazado real devuelto por OSRM a traves de RouteQuery/RouteModel.
    MapPolyline {
        path: (routeModel.status === RouteModel.Ready && routeModel.count > 0)
              ? routeModel.get(0).path : []
        visible: routeModel.status === RouteModel.Ready && routeModel.count > 0
        line.color: root.routeAffected ? "#dc2626" : root.routeColor
        line.width: root.routeTrunk ? 6 : 4
        opacity: root.routeAffected ? 0.95 : 0.8
        z: 2
    }
}
