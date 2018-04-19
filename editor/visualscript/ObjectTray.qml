import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2

Rectangle {
    property var model
    id: root
    border.color: "darkgray"
    border.width: 1
    color: "dimgray"
    clip: true
    ListView {
        anchors.fill: parent
        anchors.margins: 2
        spacing: 2
        model: root.model
        delegate: ObjectBase {
            name: model.name
            icon: model.icon
            width: parent.width
            Item {
                id: dragItem
                Drag.active: dragArea.drag.active
                Drag.dragType: Drag.Automatic
                Drag.supportedActions: Qt.CopyAction
                Drag.mimeData: {
                    "text/plain": model.name
                }
                Drag.hotSpot: Qt.point(parent.width - 10, parent.height * 0.5)
            }
            MouseArea {
                id: dragArea
                anchors.fill: parent
                drag.target: dragItem
                onPressed: parent.grabToImage(function(result) {
                    dragItem.Drag.imageSource = result.url
                })
            }
        }
    }
}
