import QtQuick
import QtQuick.Controls

Item {
    id: root

    signal closeRequested()

    property string titleText: dialogMode === "info" ? qsTr("Más información")
                                                    : (dialogMode === "call" ? qsTr("Llamar")
                                                                             : qsTr("Vehicle status"))

    Rectangle {
        anchors.fill: parent
        color: "#f8fafc"
    }

    Rectangle {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 48
        color: "#0f172a"

        Text {
            anchors.centerIn: parent
            text: root.titleText
            color: "white"
            font.bold: true
            font.pixelSize: 16
        }
    }

    Column {
        visible: dialogMode === "info"
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 16
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        spacing: 12

        Row {
            spacing: 8
            Text { width: 130; text: qsTr("Nombre:"); font.bold: true; color: "#334155" }
            Text { text: userBridge.displayName; color: "#0f172a" }
        }
        Row {
            spacing: 8
            Text { width: 130; text: qsTr("Kilometros recorridos:"); font.bold: true; color: "#334155" }
            Text { text: userBridge.kilometers.toFixed(1); color: "#0f172a" }
        }
        Row {
            spacing: 8
            Text { width: 130; text: qsTr("Antecedentes:"); font.bold: true; color: "#334155" }
            Text { width: 270; text: userBridge.antecedentes; wrapMode: Text.WordWrap; color: "#0f172a" }
        }
        Row {
            spacing: 8
            Text { width: 130; text: qsTr("Vehiculo:"); font.bold: true; color: "#334155" }
            Text { text: userBridge.vehicleId || "-"; color: "#0f172a" }
        }
        Row {
            spacing: 8
            Text { width: 130; text: qsTr("Ruta:"); font.bold: true; color: "#334155" }
            Text { text: userBridge.routeLabel || userBridge.routeId || "-"; color: "#0f172a" }
        }
        Row {
            spacing: 8
            Text { width: 130; text: qsTr("Evento:"); font.bold: true; color: "#334155" }
            Text { width: 270; text: userBridge.evento; wrapMode: Text.WordWrap; color: "#0f172a" }
        }
    }

    Column {
        visible: dialogMode === "call"
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 24
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        spacing: 12

        Text {
            text: qsTr("Llamada para una reunion con camara y audio")
            font.bold: true
            font.pixelSize: 14
            color: "#0f172a"
        }
        Text {
            width: parent.width
            text: qsTr("Consulta HTTP a IP: pendiente de implementar")
            color: "#b91c1c"
            wrapMode: Text.WordWrap
        }
        Text {
            width: parent.width
            text: qsTr("IP de destino: (sin configurar)")
            color: "#334155"
        }
    }

    Column {
        visible: dialogMode === "vehicle"
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 16
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        spacing: 12

        Row {
            spacing: 8
            Text { width: 130; text: qsTr("Placa:"); font.bold: true; color: "#334155" }
            Text { text: userBridge.placa || "-"; color: "#0f172a" }
        }
        Row {
            spacing: 8
            Text { width: 130; text: qsTr("Color:"); font.bold: true; color: "#334155" }
            Text { text: userBridge.color || "-"; color: "#0f172a" }
        }
        Row {
            spacing: 8
            Text { width: 130; text: qsTr("Marca:"); font.bold: true; color: "#334155" }
            Text { text: userBridge.marca || "-"; color: "#0f172a" }
        }
    }

    Button {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 16
        text: qsTr("Cerrar")
        onClicked: root.closeRequested()
    }
}
