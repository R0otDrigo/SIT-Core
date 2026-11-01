import QtQuick
import QtQuick.Window
Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("A new advance")

    MouseArea {
        id: mainMouseArea
        anchors.fill: parent
        signal onClicked()

        onClicked: {
            mainMouseArea.onClicked()
        }

        Cargando {
            anchors.fill: parent
        }
    }
}