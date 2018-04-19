import QtQuick 2.10
import QtQuick.Controls 2.3

ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480
    title: qsTr("Sophia Editor")

    header: ToolBar {
        contentHeight: firstButton.implicitHeight
        Row {
            anchors.fill: parent
            ToolButton {
                id: firstButton
                icon {source: "icons/solid/home.svg"}
                onClicked: stackView.set("pages/Home.qml")
            }
            ToolButton {
                icon {source: "icons/solid/dna.svg"}
                onClicked: stackView.set("pages/VisualScript.qml")
            }
            ToolSeparator {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
            }
            Item {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 300
                Label {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.margins: {left: 10}
                    text: stackView.currentItem !== null ? stackView.currentItem.title : ""
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 18
                }
            }
        }
        Row {
            anchors.fill: parent
            layoutDirection: Row.RightToLeft
            anchors.right: parent.right
            ToolButton {
                icon {source: "icons/solid/stop.svg"}
                onClicked: if (playPauseStop.checkedButton) playPauseStop.checkedButton = null
            }
            ToolButton {
                icon {source: "icons/solid/pause.svg"}
                checkable: true
                ButtonGroup.group: playPauseStop
            }
            ToolButton {
                icon {source: "icons/solid/play.svg"}
                checkable: true
                ButtonGroup.group: playPauseStop
            }
            ToolButton {
                icon {source: "icons/solid/sync.svg"}
                visible: playPauseStop.checkedButton != null
            }
            ButtonGroup {
                id: playPauseStop
            }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        focus: true
        Keys.onPressed: {
            if (stackView.currentItem.onKeyPressed) {
                stackView.currentItem.onKeyPressed(event);
            }
        }
        function set(newPage) {
            if (!currentItem || newPage !== currentItem.objectName) {
                if (currentItem && currentItem.persist) {
                    currentItem.persist();
                }
                replace(newPage, {"objectName": newPage});
                if (currentItem.restore) {
                    currentItem.restore();
                }
            }
        }
        Component.onCompleted: set("pages/VisualScript.qml") // used instead of initialItem so that objectName gets set
    }
}
