import QtQuick

Rectangle {
    id: loading
    anchors.fill: parent
    color: "#f90f0f"
    border.color: "black"

    Connections {
        target: parent
        function onClicked() {
            loading.state = "Adios"
        }
    }

    states: [
        State {
            name: "Adios"
        }
    ]
}