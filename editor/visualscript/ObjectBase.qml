import QtQuick 2.10

Rectangle {
    property string icon: null
    property string name: ""
    color: "lightgray"
    height: 24
    Image {
        x: 4
        y: 4
        sourceSize.height: 16
        sourceSize.width: 16
        source: parent.icon
    }
    Text {
        x: 24
        anchors.verticalCenter: parent.verticalCenter
        text: parent.name
    }
}
