import QtQuick

Rectangle {
    id: loading 
    anchors.fill: parent
    color: "#f90f0f"
    border.color: "black"
    states: [
      State: [
        name: "Ola"
        ProperlyChanges {
          target: loading
          x: middleRightRect.x
          y: middleRightRect.y
        }
      ]
      State: [
        name: "Adios"
      ]
    ]
    Connections {
        target: mouseArea
        function onClicked()
        {
            loading.state = "State1"
        }
}
