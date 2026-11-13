import QtQuick
import QtLocation

MapQuickItem {
    id: root

    property string stopName: ""
    property int waiting: 0
    property real saturation: 0
    property bool closed: false
    // Nombres solo con zoom cercano (o paradero cerrado): lejos se encimaban.
    property bool showLabel: true

    anchorPoint.x: sourceItem.width / 2
    anchorPoint.y: 12

    sourceItem: Item {
        id: sourceItem
        width: Math.max(120, label.width)
        height: 44

        Rectangle {
            id: dot
            x: (sourceItem.width - width) / 2
            y: 0
            width: 24
            height: 24
            radius: 12
            // Misma escala que el panel: verde < 50 %, ambar < 80 %, rojo >= 80 %.
            // El numero de personas esperando acompana siempre al color.
            color: root.closed ? "#272F42"
                               : root.saturation >= 0.8 ? "#EF4444"
                               : root.saturation >= 0.5 ? "#F59E0B"
                               : "#22C55E"
            border.color: root.closed ? "#EF4444" : "white"
            border.width: root.closed ? 3 : 2

            Text {
                anchors.centerIn: parent
                text: root.closed ? "\u2715" : root.waiting
                color: root.saturation >= 0.5 && root.saturation < 0.8 && !root.closed ? "#0F172A" : "white"
                font.pixelSize: 10
                font.bold: true
            }
        }

        Text {
            id: label
            anchors.top: dot.bottom
            anchors.topMargin: 1
            anchors.horizontalCenter: dot.horizontalCenter
            visible: root.showLabel || root.closed
            text: root.closed ? qsTr("%1 (cerrado)").arg(root.stopName) : root.stopName
            color: "#0F172A"
            font.pixelSize: 10
            style: Text.Outline
            styleColor: "white"
        }
    }
}
