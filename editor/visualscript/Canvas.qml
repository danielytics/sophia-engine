import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2

Item {
    id: root
    property var objectTypesModel
    property var _objectTypes
    property var _objects
    property var _connections: []
    property var _selectedConnection: -1

    Rectangle {
        anchors.fill: parent
        anchors.margins: 2
        border.color: "gray"
        border.width: 1
        color: "lightgray"

        DropArea {
            anchors.fill: parent
            onDropped: {
                if ((drop.formats + "").indexOf("x-applicatoin/object") !== -1) {
                    var model = root._objectTypes[drop.text];
                    var newObject = object.createObject(container, {"x": drop.x - 110, "y": drop.y - (16 + (model.inputs.count > 0 ? 6 : 0)), "model": model});
                    root._objects[newObject] = newObject;
                    container.selected = newObject;
                    drop.accept(Qt.CopyAction);
                }
            }
        }

        Flickable {
            id: container
            property var selected: null
            anchors.fill: parent
            anchors.margins: 2
            ScrollBar.vertical: ScrollBar {
                parent: container.parent
                anchors.top: container.top
                anchors.left: container.right
                anchors.bottom: container.bottom
            }
            ScrollBar.horizontal: ScrollBar { }
            clip: true

            Canvas {
                id: canvas
                anchors.fill: parent
                contextType: "2d"
                onPaint: {
                    context.clearRect(0, 0, width, height);
                    context.lineWidth = 5;
                    for (var idx in root._connections) {
                        var connection = root._connections[idx];
                        if (connection.selected) {
                            context.strokeStyle = Qt.rgba(1.0, 0.0, 0.0);
                        } else {
                            context.strokeStyle = Qt.rgba(.4,.6,.8);
                        }
                        context.beginPath()
                        context.moveTo(connection.start.x, connection.start.y);
                        context.lineTo(connection.end.x, connection.end.y);
                        context.stroke();
                    }
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: container.selected = null
                Component {
                    id: object
                    Object {
                        isSelected: container.selected === this
                        onObjectSelected: container.selected = this
                        onConnectionSelected: {
                            if (id === -1) {
                                // New connection
                                console.log("New connection");
                                id = root._connections.length;
                                var start = canvas.mapFromGlobal(startPoint.x, startPoint.y);
                                root._connections.push({
                                    start: start,
                                    end: start,
                                    selected: true
                                });
                                addConnection(id, connector); // Tell the object about the new connection id
                            }
                            console.log("Selected connection", id);
                            root._selectedConnection = id;
                            canvas.requestPaint();
                        }
                        onConnectionMoved: {
                            if (root._selectedConnection > -1) {
                                var connection = root._connections[root._selectedConnection];
                                connection.end = canvas.mapFromGlobal(point.x, point.y);
                                canvas.requestPaint();
                            }
                        }
                        onConnectionDeselected: root._selectedConnection = -1;
                    }
                }
            }

        }
    }

    onObjectTypesModelChanged: {
        var types = {};
        for (var idx=0; idx<objectTypesModel.count; ++idx) {
            var type = objectTypesModel.get(idx);
            types[type.name] = type;
        }
        _objectTypes = types;
    }

    Component.onCompleted: {
        _objects = {};
    }

    function deleteSelected() {
        if (container.selected !== null) {
            delete root._objects[container.selected];
            container.selected.destroy();
            container.selected = null;
        }
    }

    function serialise() {
        var data = [];
        for (var key in root._objects) {
            var object = root._objects[key];
            data.push({x: object.x, y: object.y, type: object.model.name});
        }
        return data;
    }
    function unserialise(data) {

    }
}
