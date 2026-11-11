import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    visible: true
    width: 1180
    height: 760
    title: qsTr("TEO-TO · Simulador SIT Arequipa (Prototipo 1)")

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 8

            Button {
                text: sim.running ? qsTr("Pausar") : qsTr("Iniciar")
                onClicked: sim.toggleRunning()
            }
            Button {
                text: qsTr("Reiniciar")
                onClicked: sim.reset()
            }

            Label { text: qsTr("Velocidad") }

            ComboBox {
                id: speedBox
                textRole: "label"
                valueRole: "value"
                currentIndex: 1
                model: ListModel {
                    ListElement { label: "0.5x"; value: 0.5 }
                    ListElement { label: "1x";   value: 1.0 }
                    ListElement { label: "2x";   value: 2.0 }
                    ListElement { label: "5x";   value: 5.0 }
                    ListElement { label: "10x";  value: 10.0 }
                }
                onActivated: sim.speedMultiplier = valueAt(currentIndex)
            }

            Item { Layout.fillWidth: true }

            Label { text: qsTr("Esquemático") }
            Switch {
                checked: sim.mapMode === "offline"
                onToggled: sim.mapMode = checked ? "offline" : "online"
            }

            Label {
                text: qsTr("Mapa: %1").arg(sim.mapMode === "online" ? qsTr("en línea (OSM)")
                                                                   : qsTr("esquemático"))
                color: sim.mapMode === "online" ? "#2e7d32" : "#b26a00"
            }
        }
    }

    Loader {
        anchors.fill: parent
        sourceComponent: sim.mapMode === "online" ? onlineMap : offlineMap
    }

    Component { id: onlineMap;  MapView {} }
    Component { id: offlineMap; MapFallback {} }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent
            Label {
                text: sim.statusText
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
        }
    }
}
