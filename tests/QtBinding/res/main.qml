import QtQuick 2.9
import MSG 1.0

MSGWindow {
    id: window
    width: 400
    height: 400
    visible: true
    color: "steelblue"
    MSGBindingTestItem {
        anchors.fill: parent
    }
    Text {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        text: "This is a test"
        font.family: "Helvetica"
        font.pointSize: 24
        color: "white"
    }
}
