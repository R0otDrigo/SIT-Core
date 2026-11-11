import QtQuick
import QtPositioning

// Mapa esquematico de respaldo (RF-35): dibuja el mismo modelo sobre un lienzo
// propio con una proyeccion lineal simple, sin depender de la red.
Item {
    id: root

    // Encuadre aproximado de Arequipa (ver requerimientos §9.1).
    property real minLat: -16.47
    property real maxLat: -16.33
    property real minLon: -71.61
    property real maxLon: -71.47

    function toX(lon) { return (lon - root.minLon) / (root.maxLon - root.minLon) * width }
    function toY(lat) { return (root.maxLat - lat) / (root.maxLat - root.minLat) * height }

    Rectangle {
        anchors.fill: parent
        color: "#eef2f5"
    }

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            ctx.fillStyle = "#eef2f5"
            ctx.fillRect(0, 0, width, height)

            // Ruta
            var path = sim.routePath
            if (path.length > 1) {
                ctx.beginPath()
                ctx.strokeStyle = "#1565c0"
                ctx.lineWidth = 4
                ctx.lineJoin = "round"
                for (var i = 0; i < path.length; ++i) {
                    var x = root.toX(path[i].longitude)
                    var y = root.toY(path[i].latitude)
                    if (i === 0)
                        ctx.moveTo(x, y)
                    else
                        ctx.lineTo(x, y)
                }
                ctx.stroke()
            }

            // Paraderos
            var stops = sim.stops
            ctx.font = "12px sans-serif"
            for (var s = 0; s < stops.length; ++s) {
                var sx = root.toX(stops[s].lon)
                var sy = root.toY(stops[s].lat)
                ctx.beginPath()
                ctx.fillStyle = "#2e7d32"
                ctx.arc(sx, sy, 7, 0, Math.PI * 2)
                ctx.fill()
                ctx.fillStyle = "#222222"
                ctx.fillText(stops[s].name, sx + 11, sy + 4)
            }

            // Bus
            var bx = root.toX(sim.busPosition.longitude)
            var by = root.toY(sim.busPosition.latitude)
            ctx.fillStyle = "#e53935"
            ctx.fillRect(bx - 15, by - 9, 30, 18)
            ctx.fillStyle = "white"
            ctx.font = "bold 9px sans-serif"
            ctx.fillText("BUS", bx - 11, by + 3)
        }
    }

    Text {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 12
        text: qsTr("Modo esquemático — sin conexión al mapa base")
        color: "#555555"
    }

    Connections {
        target: sim
        function onStateChanged() { canvas.requestPaint() }
    }

    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()
}
