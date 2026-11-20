import QtQuick
import QtQuick.Controls
import "Theme.js" as Theme

// Vista de reserva. La reunión real usa MeetingDialog (Qt Widgets), que abre
// la vista previa de cámara en una ventana nativa.
Item {
    id: root

    Rectangle {
        anchors.fill: parent
        color: Theme.bg

        Column {
            anchors.centerIn: parent
            width: Math.min(360, parent.width - 48)
            spacing: 12

            Text {
                width: parent.width
                text: qsTr("Reunión")
                color: Theme.text
                font.pixelSize: 22
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                width: parent.width
                text: qsTr("La vista de la reunión se abre en una ventana independiente con la cámara y el micrófono.")
                color: Theme.muted
                font.pixelSize: 12
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
