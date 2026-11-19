import QtQuick
import QtLocation

MapQuickItem {
    id: root

    property string stopName: ""
    property int waiting: 0
    property real saturation: 0
    property bool closed: false

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
            color: root.closed ? "#495057"
                               : root.saturation >= 0.8 ? "#d62828"
                               : root.saturation >= 0.5 ? "#f9c74f"
                               : "#43aa8b"
            border.color: root.closed ? "#b91c1c" : "white"
            border.width: root.closed ? 3 : 2

            Text {
                anchors.centerIn: parent
                text: root.waiting
                color: root.saturation >= 0.5 && !root.closed ? "#1e293b" : "white"
                font.pixelSize: 8
                font.bold: true
            }
        }

        Text {
            id: label
            anchors.top: dot.bottom
            anchors.topMargin: 1
            anchors.horizontalCenter: dot.horizontalCenter
            text: root.stopName
            color: "#0f172a"
            font.pixelSize: 9
            style: Text.Outline
            styleColor: "white"
        }
    }
}
