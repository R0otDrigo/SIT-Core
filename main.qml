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
        // We define a custom signal because standard MouseArea.clicked
        // is an event, not a signal that Connections always catches easily
        signal onClicked()

        onClicked: {
            onClicked() // Trigger the signal
        }

        loading {
            // Pass the ID of the mouse area into the property
            externalMouseArea: mainMouseArea

        }
}