import QtQuick
import QtQuick.Controls
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
    property bool showStats: false
    property bool incomingCall: false
    property string statsText: ""
    property string callName: ""
    property string callStatus: ""

    signal clicked()
    signal callRequested()
    signal callAnswered()

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

        MouseArea {
            anchors.fill: parent
            onClicked: root.clicked()
        }

        Rectangle {
            id: popup
            visible: root.showStats || root.incomingCall
            anchors.bottom: parent.top
            anchors.bottomMargin: 6
            anchors.horizontalCenter: parent.horizontalCenter
            width: 232
            height: root.incomingCall ? 92 : 104
            radius: 6
            color: "#ffffff"
            border.color: root.incomingCall ? "#16a34a" : "#dc2626"
            border.width: 2
            z: 10

            Column {
                anchors.fill: parent
                anchors.margins: 6
                spacing: 4

                Text {
                    width: parent.width
                    text: root.incomingCall
                          ? qsTr("Llamada de %1").arg(root.callName || "Usuario")
                          : root.statsText
                    color: "#0f172a"
                    font.pixelSize: 9
                    font.bold: root.incomingCall
                    wrapMode: Text.WordWrap
                    maximumLineCount: 2
                    elide: Text.ElideRight
                }

                Text {
                    width: parent.width
                    visible: root.incomingCall && root.callStatus.length > 0
                    text: root.callStatus
                    color: "#64748b"
                    font.pixelSize: 8
                    wrapMode: Text.WordWrap
                    maximumLineCount: 1
                    elide: Text.ElideRight
                }

                Button {
                    width: parent.width
                    height: 26
                    text: root.incomingCall ? qsTr("Atender Llamada") : qsTr("Llamar")
                    palette.buttonText: "white"
                    background: Rectangle {
                        radius: 4
                        color: root.incomingCall ? "#16a34a" : "#dc2626"
                    }
                    onClicked: {
                        if (root.incomingCall) root.callAnswered()
                        else root.callRequested()
                    }
                }
            }
        }
    }
}
