import QtQuick
import QtQuick.Controls
import QtLocation
import "Theme.js" as Theme

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

    readonly property real occupancyRatio: capacity > 0 ? Math.min(1, passengers / capacity) : 0
    readonly property int occupancyPercent: Math.round(occupancyRatio * 100)
    readonly property color occupancyColor: Theme.occupancyColor(occupancyRatio)

    signal clicked()
    signal callRequested()
    signal callAnswered()

    anchorPoint.x: bus.width / 2
    anchorPoint.y: bus.height / 2

    sourceItem: Rectangle {
        id: bus
        width: 30 + Math.min(22, Math.floor(root.capacity / 8))
        height: 18
        radius: 5
        color: root.routeColor
        border.color: "white"
        border.width: 2
        opacity: root.dwelling ? 0.82 : 1.0

        Rectangle {
            anchors.fill: parent
            anchors.margins: 2
            radius: 3
            color: "transparent"
            border.color: Qt.rgba(1, 1, 1, 0.35)
            border.width: 1
        }

        Text {
            anchors.centerIn: parent
            text: root.passengers
            color: "white"
            font.pixelSize: 8
            font.bold: true
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.clicked()
        }

        Rectangle {
            id: popup
            visible: root.showStats || root.incomingCall
            anchors.bottom: parent.top
            anchors.bottomMargin: 8
            anchors.horizontalCenter: parent.horizontalCenter
            width: 274
            height: popupColumn.implicitHeight + 24
            radius: Theme.radius
            color: Theme.surface
            border.color: root.incomingCall ? Theme.success : Theme.border
            border.width: root.incomingCall ? 2 : 1
            z: 20

            Column {
                id: popupColumn
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 12
                spacing: 7

                Row {
                    width: parent.width
                    spacing: 6
                    Text {
                        text: root.incomingCall
                              ? qsTr("Llamada de %1").arg(root.callName || qsTr("Usuario"))
                              : qsTr("Bus #%1").arg(root.vehicleId)
                        color: Theme.text
                        font.pixelSize: 12
                        font.bold: true
                        elide: Text.ElideRight
                        width: parent.width - routeBadge.width - 8
                    }
                    Rectangle {
                        id: routeBadge
                        width: routeBadgeText.implicitWidth + 14
                        height: 20
                        radius: 10
                        color: "#EFF6FF"
                        Text {
                            id: routeBadgeText
                            anchors.centerIn: parent
                            text: root.routeId || "—"
                            color: Theme.primaryDark
                            font.pixelSize: 9
                            font.bold: true
                        }
                    }
                }

                Text {
                    width: parent.width
                    visible: !root.incomingCall
                    text: root.statsText
                    color: Theme.muted
                    font.pixelSize: 10
                    wrapMode: Text.WordWrap
                    maximumLineCount: 3
                    elide: Text.ElideRight
                }

                Text {
                    width: parent.width
                    visible: root.incomingCall && root.callStatus.length > 0
                    text: root.callStatus
                    color: Theme.muted
                    font.pixelSize: 10
                    wrapMode: Text.WordWrap
                    maximumLineCount: 2
                    elide: Text.ElideRight
                }

                Row {
                    width: parent.width
                    Text {
                        text: qsTr("Ocupación")
                        color: Theme.muted
                        font.pixelSize: 9
                        font.bold: true
                    }
                    Item { width: parent.width - occupancyLabel.implicitWidth - 80; height: 1 }
                    Text {
                        id: occupancyLabel
                        text: root.occupancyPercent + "%"
                        color: root.occupancyColor
                        font.pixelSize: 10
                        font.bold: true
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 7
                    radius: 4
                    color: "#E2E8F0"
                    Rectangle {
                        width: parent.width * root.occupancyRatio
                        height: parent.height
                        radius: 4
                        color: root.occupancyColor
                    }
                }

                Button {
                    id: actionButton
                    width: parent.width
                    height: 30
                    text: root.incomingCall ? qsTr("Atender llamada") : qsTr("Llamar")
                    onClicked: {
                        if (root.incomingCall) root.callAnswered()
                        else root.callRequested()
                    }
                    contentItem: Text {
                        text: actionButton.text
                        color: "white"
                        font.pixelSize: 11
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        radius: 7
                        color: root.incomingCall ? Theme.success
                                                 : (actionButton.down ? Theme.primaryDark : Theme.primary)
                    }
                }
            }
        }
    }
}
