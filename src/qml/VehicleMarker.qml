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

    // Seleccionado = showStats: borde ambar grueso y encima del resto.
    z: showStats || incomingCall ? 10 : 1

    sourceItem: Rectangle {
        id: bus
        width: 28 + Math.min(20, Math.floor(root.capacity / 8))
        height: 18
        radius: 6
        color: root.routeColor
        border.color: root.showStats ? "#F59E0B" : "#0F172A"
        border.width: root.showStats ? 3 : 1
        opacity: root.dwelling ? 0.75 : 1.0

        Text {
            anchors.centerIn: parent
            text: root.passengers
            color: "white"
            style: Text.Outline
            styleColor: "#0F172A"
            font.pixelSize: 10
            font.bold: true
        }

        MouseArea {
            // Zona de clic de al menos 24 px aunque el marcador sea mas pequeno.
            anchors.centerIn: parent
            width: Math.max(parent.width, 24)
            height: Math.max(parent.height, 24) + 8
            cursorShape: Qt.PointingHandCursor
            onClicked: root.clicked()
        }

        Rectangle {
            id: popup
            visible: root.showStats || root.incomingCall
            anchors.bottom: parent.top
            anchors.bottomMargin: 8
            anchors.horizontalCenter: parent.horizontalCenter
            width: 240
            height: popupColumn.implicitHeight + 20
            radius: 8
            color: "#1B2336"
            border.color: root.incomingCall ? "#22C55E" : "#334155"
            border.width: root.incomingCall ? 2 : 1
            z: 10

            Column {
                id: popupColumn
                anchors.fill: parent
                anchors.margins: 10
                spacing: 6

                Text {
                    width: parent.width
                    text: root.incomingCall
                          ? qsTr("Llamada de %1").arg(root.callName || qsTr("Usuario"))
                          : root.statsText
                    color: "#F8FAFC"
                    font.pixelSize: 12
                    font.bold: root.incomingCall
                    wrapMode: Text.WordWrap
                    maximumLineCount: 3
                    elide: Text.ElideRight
                }

                Text {
                    width: parent.width
                    visible: root.incomingCall && root.callStatus.length > 0
                    text: root.callStatus
                    color: "#94A3B8"
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                    maximumLineCount: 1
                    elide: Text.ElideRight
                }

                Button {
                    id: callButton
                    width: parent.width
                    height: 30
                    text: root.incomingCall ? qsTr("Atender llamada") : qsTr("Llamar al conductor")
                    contentItem: Text {
                        text: callButton.text
                        color: root.incomingCall ? "#0F172A" : "#F8FAFC"
                        font.bold: true
                        font.pixelSize: 12
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        radius: 6
                        color: root.incomingCall ? (callButton.down ? "#16A34A" : "#22C55E")
                                                 : (callButton.down ? "#334155" : "#272F42")
                        border.color: root.incomingCall ? "transparent" : "#334155"
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
