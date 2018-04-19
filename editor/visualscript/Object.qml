import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2

Rectangle {
    readonly property int _connectorSize: 8
    property var model
    property bool isSelected
    signal objectSelected
    signal connectionSelected(int id, int connector, var startPoint)
    signal connectionMoved(var point)
    signal connectionDeselected()

    property var _connections

    id: root
    border.color: isSelected ? "dodgerblue" : "gray"
    border.width: isSelected ? 2 : 1
    color: "whitesmoke"
    x: 10
    y: 10
    width: 120
    height: 24 + (model.inputs.count > 0 ? _connectorSize : 2)  + (model.outputs.count > 0 ? _connectorSize : 2)
    radius: 3

    MouseArea {
        anchors.fill: parent
        cursorShape: drag.active ? Qt.SizeAllCursor : Qt.PointingHandCursor
        drag.target: root
        onPressed: parent.objectSelected()
        Row {
            anchors.top: parent.top
            anchors.topMargin: 1
            anchors.left: parent.left
            anchors.leftMargin: 4
            property bool attachTop: true
            Repeater {
                model: root.model.inputs
                delegate: connectorComponent
            }
        }
        Row {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 1
            anchors.left: parent.left
            anchors.leftMargin: 4
            property bool attachTop: false
            Repeater {
                model: root.model.outputs
                delegate: connectorComponent
            }
        }
        Component {
            id: connectorComponent
            Control {
                id: connector
                property bool attachTop: parent.attachTop
                property int connectionNumber: index
                width: _connectorSize
                height: _connectorSize
                hoverEnabled: true
                ToolTip.delay: 300
                ToolTip.visible: hovered
                ToolTip.text: model.name
                clip: true
                Rectangle {
                    radius: _connectorSize * 0.3
                    y: parent.attachTop ? -(_connectorSize+2) : 2
                    height: _connectorSize * 2
                    width: _connectorSize
                    border.color: "dimgray"
                    border.width: 1
                    color: "gray"

                    MouseArea {
                        id: connectorMouse
                        anchors.fill: parent
                        onPressed: {
                            var connectionId = connector.connectionNumber;
                            if (!attachTop) {
                                // output connections are stored negative
                                connectionId = -connectionId;
                            }
                            var id = root._connections[connectionId];
                            var startPoint = mapToGlobal(x, y);
                            connectionSelected(id !== undefined ? id : -1, connectionId, startPoint); // null means new connections, let the slot decide its id (must notify the object about it later!)
                            hoverEnabled = true;
                        }
                        onPositionChanged: {
                            connectionMoved(mapToGlobal(mouseX, mouseY));
                        }
                        onReleased: {
                            connectionDeselected();
                            hoverEnabled = false;
                        }
                    }
                }
            }
        }
    }


    ObjectBase {
        id: label
        anchors.top: parent.top
        anchors.topMargin: root.model.inputs.count > 0 ? _connectorSize : 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.model.outputs.count > 0 ? _connectorSize : 2
        anchors.left: parent.left
        anchors.leftMargin: 2
        anchors.right: parent.right
        anchors.rightMargin: 2
        name: model.name
        icon: model.icon
    }


    Component.onCompleted: {
        _connections = {};
    }

    function addConnection(id, connector) {
        _connections[connector] = id;
    }
}
