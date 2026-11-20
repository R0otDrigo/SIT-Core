import QtQuick
import QtLocation
import "Theme.js" as Theme

MapQuickItem {
    id: root

    property string stopName: ""
    property int waiting: 0
    property real saturation: 0
    property bool closed: false

    readonly property color stopColor: closed ? "#64748B"
                                             : saturation >= 0.8 ? Theme.danger
                                             : saturation >= 0.5 ? Theme.warning
                                             : Theme.success

    anchorPoint.x: sourceItem.width / 2
    anchorPoint.y: 13

    sourceItem: Item {
        id: sourceItem
        width: Math.max(120, labelBackground.width + 10)
        height: 46

        Rectangle {
            id: dot
            x: (sourceItem.width - width) / 2
            y: 0
            width: 26
            height: 26
            radius: 13
            color: root.stopColor
            border.color: "white"
            border.width: 2

            Text {
                anchors.centerIn: parent
                text: root.waiting
                color: "white"
                font.pixelSize: 8
                font.bold: true
            }

            Rectangle {
                visible: root.closed
                anchors.centerIn: parent
                width: 16
                height: 2
                radius: 1
                color: "white"
            }
        }

        Rectangle {
            id: labelBackground
            anchors.top: dot.bottom
            anchors.topMargin: 3
            anchors.horizontalCenter: dot.horizontalCenter
            width: label.implicitWidth + 8
            height: 16
            radius: 8
            color: "#F2FFFFFF"
            border.color: "#E2E8F0"
            border.width: 1

            Text {
                id: label
                anchors.centerIn: parent
                text: root.stopName
                color: Theme.text
                font.pixelSize: 9
                font.bold: true
            }
        }
    }
}
