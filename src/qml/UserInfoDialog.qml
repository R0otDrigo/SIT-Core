import QtQuick
import QtQuick.Controls

Item {
    id: root

    signal closeRequested()

    property string titleText: dialogMode === "info" ? qsTr("Más información")
                                                    : (dialogMode === "call" ? qsTr("Llamar")
                                                                             : qsTr("Estado del vehículo"))

    // Fila etiqueta / valor con la paleta de la app.
    component Field: Row {
        property alias label: fieldLabel.text
        property alias value: fieldValue.text
        spacing: 12
        Text { id: fieldLabel; width: 140; color: "#94A3B8"; font.pixelSize: 13 }
        Text { id: fieldValue; width: 270; color: "#F8FAFC"; font.pixelSize: 13; wrapMode: Text.WordWrap }
    }

    Rectangle {
        anchors.fill: parent
        color: "#0F172A"
    }

    Rectangle {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 48
        color: "#1B2336"

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
            text: root.titleText
            color: "#F8FAFC"
            font.bold: true
            font.pixelSize: 16
        }
    }

    Column {
        visible: dialogMode === "info"
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 20
        spacing: 10

        Field { label: qsTr("Nombre"); value: userBridge.displayName }
        Field { label: qsTr("Km recorridos"); value: userBridge.kilometers.toFixed(1) }
        Field { label: qsTr("Antecedentes"); value: userBridge.antecedentes || "-" }
        Field { label: qsTr("Vehículo"); value: userBridge.vehicleId || "-" }
        Field { label: qsTr("Ruta"); value: userBridge.routeLabel || userBridge.routeId || "-" }
        Field { label: qsTr("Evento"); value: userBridge.evento || "-" }
    }

    Column {
        visible: dialogMode === "call"
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 20
        spacing: 10

        Text {
            text: qsTr("Reunión con cámara y audio")
            font.bold: true
            font.pixelSize: 14
            color: "#F8FAFC"
        }
        Text {
            width: parent.width
            text: qsTr("Consulta HTTP a IP: pendiente de implementar")
            color: "#F59E0B"
            wrapMode: Text.WordWrap
        }
        Field { label: qsTr("IP de destino"); value: qsTr("(sin configurar)") }
    }

    Column {
        visible: dialogMode === "vehicle"
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 20
        spacing: 10

        Field { label: qsTr("Placa"); value: userBridge.placa || "-" }
        Field { label: qsTr("Color"); value: userBridge.color || "-" }
        Field { label: qsTr("Marca"); value: userBridge.marca || "-" }
    }

    Button {
        id: closeButton
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 16
        width: 96
        height: 32
        text: qsTr("Cerrar")
        contentItem: Text {
            text: closeButton.text
            color: "#F8FAFC"
            font.pixelSize: 13
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            radius: 6
            color: closeButton.down ? "#334155" : "#272F42"
            border.color: "#334155"
        }
        onClicked: root.closeRequested()
    }
}
