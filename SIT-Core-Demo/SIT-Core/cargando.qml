import QtQuick

// Pantalla de carga de SIT-Core.
// Propiedades controladas desde C++ (LoadingDialog con SIT_QML_LOADING):
//   progress: 0.0 - 1.0     message: texto principal
Rectangle {
    id: root
    width: 360
    height: 200
    color: "#FFFFFF"
    radius: 14
    border.color: "#E5E7EB"

    property real progress: 0.0
    property string message: "Cargando..."
    property string detail: ""

    Item {
        id: spin
        width: 64
        height: 64
        y: 22
        anchors.horizontalCenter: parent.horizontalCenter

        RotationAnimator on rotation {
            from: 0; to: 360; duration: 1100
            loops: Animation.Infinite
            running: true
        }
        Repeater {
            model: 12
            Rectangle {
                width: 9; height: 9; radius: 4.5
                color: "#3B82F6"
                opacity: (index + 1) / 12
                x: spin.width / 2 - 4.5 + 26 * Math.cos(index * Math.PI / 6)
                y: spin.height / 2 - 4.5 + 26 * Math.sin(index * Math.PI / 6)
            }
        }
    }

    Text {
        y: 100
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        text: root.message
        color: "#1E3A8A"
        font.pixelSize: 20
        font.bold: true
    }
    Text {
        y: 128
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        text: root.detail
        color: "#6B7280"
        font.pixelSize: 12
    }
    Rectangle {
        x: 40; y: 164
        width: parent.width - 80; height: 10; radius: 5
        color: "#E5E7EB"
        Rectangle {
            width: parent.width * root.progress
            height: parent.height; radius: 5
            color: "#3B82F6"
        }
    }
}
