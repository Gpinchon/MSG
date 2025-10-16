import QtQuick 2.9
import MSG 1.0

MSGWindow {
    id: window
    width: 800
    height: 800
    visible: true
    color: "steelblue"
    MSGBindingTestItem {
        // width: 600
        // height: 600
        visible: true
        anchors.centerIn: parent
        anchors.fill: parent
    }
}
