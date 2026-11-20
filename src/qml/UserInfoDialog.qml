import QtQuick
import QtQuick.Controls
import "Theme.js" as Theme

Item {
    id: root

    signal closeRequested()

    property string titleText: dialogMode === "info" ? qsTr("Más información")
                                                    : (dialogMode === "call" ? qsTr("Llamar")
                                                                             : qsTr("Estado del vehículo"))

    component Field: Item {
        width: parent.width
        height: Math.max(labelText.implicitHeight, valueText.implicitHeight)
        property string label: ""
        property string value: ""

        Text {
            id: labelText
            width: 130
            text: parent.label
            color: Theme.muted
            font.pixelSize: 12
            font.bold: true
            wrapMode: Text.WordWrap
        }
        Text {
            id: valueText
            x: 142
            width: parent.width - 142
            text: parent.value
            color: Theme.text
            font.pixelSize: 12
            wrapMode: Text.WordWrap
        }
    }

    component SectionTitle: Text {
        width: parent.width
        color: Theme.muted
        font.pixelSize: 10
        font.bold: true
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.bg
    }

    Rectangle {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 58
        color: Theme.surface
        border.color: Theme.border
        border.width: 1

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            text: root.titleText
            color: Theme.text
            font.pixelSize: 16
            font.bold: true
        }
        Button {
            id: headerClose
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Cerrar")
            onClicked: root.closeRequested()
            contentItem: Text {
                text: headerClose.text
                color: Theme.muted
                font.pixelSize: 11
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                radius: 7
                color: headerClose.down ? "#F1F5F9" : "transparent"
            }
        }
    }

    Flickable {
        id: flick
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: footer.top
        anchors.margins: 16
        clip: true
        contentHeight: contentColumn.implicitHeight
        boundsBehavior: Flickable.StopAtBounds

        Column {
            id: contentColumn
            width: flick.width
            spacing: 12

            // --- Más información ---
            Rectangle {
                visible: dialogMode === "info"
                width: parent.width
                height: infoColumn.implicitHeight + 28
                radius: Theme.radius
                color: Theme.surface
                border.color: Theme.border
                border.width: 1

                Column {
                    id: infoColumn
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 14
                    spacing: 10

                    SectionTitle { text: qsTr("CONDUCTOR") }
                    Field { label: qsTr("Nombre"); value: userBridge.displayName || "—" }
                    Field { label: qsTr("Kilómetros"); value: userBridge.kilometers.toFixed(1) }
                    Field { label: qsTr("Antecedentes"); value: userBridge.antecedentes || qsTr("Sin antecedentes") }
                }
            }

            Rectangle {
                visible: dialogMode === "info"
                width: parent.width
                height: infoVehicleColumn.implicitHeight + 28
                radius: Theme.radius
                color: Theme.surface
                border.color: Theme.border
                border.width: 1

                Column {
                    id: infoVehicleColumn
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 14
                    spacing: 10

                    SectionTitle { text: qsTr("VEHÍCULO Y SERVICIO") }
                    Field { label: qsTr("Vehículo"); value: userBridge.vehicleId || "—" }
                    Field { label: qsTr("Ruta"); value: userBridge.routeLabel || userBridge.routeId || "—" }
                    Field { label: qsTr("Evento"); value: userBridge.evento || qsTr("Sin eventos activos") }
                }
            }

            // --- Llamada ---
            Rectangle {
                visible: dialogMode === "call"
                width: parent.width
                height: callColumn.implicitHeight + 28
                radius: Theme.radius
                color: Theme.surface
                border.color: Theme.border
                border.width: 1

                Column {
                    id: callColumn
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 16
                    spacing: 12

                    Text {
                        width: parent.width
                        text: qsTr("Reunión con el administrador")
                        color: Theme.text
                        font.pixelSize: 15
                        font.bold: true
                    }
                    Text {
                        width: parent.width
                        text: qsTr("La llamada se abrirá en una ventana aparte con la vista previa de cámara y micrófono.")
                        color: Theme.muted
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                    }
                    Rectangle {
                        width: parent.width
                        height: 1
                        color: Theme.border
                    }
                    Field { label: qsTr("Interlocutor"); value: qsTr("Administrador") }
                    Field { label: qsTr("Vehículo"); value: userBridge.vehicleId || "—" }
                }
            }

            // --- Estado del vehículo ---
            Rectangle {
                visible: dialogMode === "vehicle"
                width: parent.width
                height: vehicleColumn.implicitHeight + 28
                radius: Theme.radius
                color: Theme.surface
                border.color: Theme.border
                border.width: 1

                Column {
                    id: vehicleColumn
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 14
                    spacing: 10

                    SectionTitle { text: qsTr("DATOS DEL VEHÍCULO") }
                    Field { label: qsTr("Placa"); value: userBridge.placa || "—" }
                    Field { label: qsTr("Marca"); value: userBridge.marca || "—" }
                    Field { label: qsTr("Tipo"); value: userBridge.vehicleType || "—" }
                    Field { label: qsTr("Ruta"); value: userBridge.routeLabel || userBridge.routeId || "—" }
                    Field { label: qsTr("Pasajeros"); value: userBridge.passengers + " / " + userBridge.capacity }
                }
            }
        }
    }

    Rectangle {
        id: footer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 62
        color: Theme.surface
        border.color: Theme.border
        border.width: 1

        Button {
            id: footerClose
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Cerrar")
            onClicked: root.closeRequested()
            contentItem: Text {
                text: footerClose.text
                color: "white"
                font.pixelSize: 12
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                radius: 8
                color: footerClose.down ? Theme.primaryDark : Theme.primary
            }
        }
    }
}
