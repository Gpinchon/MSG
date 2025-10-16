import QtQuick 2.9
import QtQuick.Controls
import MSG 1.0

MSGWindow {
    id: window
    width: 400
    height: 400
    visible: true
    color: "steelblue"
    Text {
        anchors.centerIn: parent
        text: "THIS IS QT BACKGROUND"
        color: "white"
        font.family: "Helvetica"
        font.pointSize: 24
    }
    MSGBindingTestItem {
        id: msgItem
        anchors.fill: parent
    }
    Button {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        text: "Toggle skybox"
        font.family: "Helvetica"
        font.pointSize: 24
        onClicked: msgItem.skyboxEnabled = !msgItem.skyboxEnabled
    }
}
