import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2

Page {
    id: root
    title: "Visual Script"

    property point mousePosition

    function onKeyPressed (event) {
        if (event.key === Qt.Key_Space) {
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 1
        Rectangle {
            Layout.fillHeight: true
            width: 80
            border.color: "darkgray"
            border.width: 1
            color: "dimgray"
            clip: true
            ListView {
                anchors.fill: parent
                anchors.margins: 2
                spacing: 2
                ListModel {
                    id: nodesModel
                    ListElement {
                        name: "event"
                        icon: "/icons/solid/long-arrow-alt-down.svg"
                    }
                    ListElement {
                        name: "timer"
                        icon: "/icons/regular/clock.svg"
                    }
                }
                model: nodesModel
                delegate: Rectangle {
                    color: "lightgray"
                    width: parent.width
                    height: 24
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
                    Image {
                        x: 4
                        y: 4
                        sourceSize.height: 16
                        sourceSize.width: 16
                        source: model.icon
                    }
                    Text {
                        x: 24
                        anchors.verticalCenter: parent.verticalCenter
                        text: model.name
                    }
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Rectangle {
                anchors.fill: parent
                anchors.margins: 2
                border.color: "gray"
                border.width: 1
                color: "lightgray"

                DropArea {
                    anchors.fill: parent
                    onDropped: {
                        container.selected = node.createObject(container, {"x": drop.x - 50, "y": drop.y - 10, "nodeType": drop.text});
                        drop.accept(Qt.CopyAction);
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

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        onClicked: container.selected = null
                        onPositionChanged: {
                            root.mousePosition = Qt.point(mouseX, mouseY);
                        }
                    }

                    Component {
                        id: node
                        Rectangle {
                            id : nodeRoot
                            property bool selected: container.selected === this
                            property string nodeType
                            border.color: selected ? "dodgerblue" : "gray"
                            border.width: selected ? 2 : 1
                            color: "whitesmoke"
                            x: 10
                            y: 10
                            width: 100
                            height: 20
                            radius: 3

                            Text {
                                anchors.fill: parent
                                text: nodeRoot.nodeType
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: drag.active ? Qt.SizeAllCursor : Qt.PointingHandCursor
                                drag.target: nodeRoot
                                onPressed: container.selected = parent;
                            }
                        }
                    }

                }
            }
        }
    }
}
