import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import "../visualscript" as VS

Page {
    id: root
    title: "Visual Script"

    VS.ObjectsModel {
        id: objects
    }

    RowLayout {
        anchors.fill: parent
        spacing: 1
        VS.ObjectTray {
            Layout.fillHeight: true
            width: 80
            model: objects
        }
        VS.Canvas {
            id: codeCanvas
            Layout.fillWidth: true
            Layout.fillHeight: true
            objectTypesModel: objects
        }
    }

    function onKeyPressed (event) {
        switch (event.key) {
        case Qt.Key_Backspace:
        case Qt.Key_Delete:
            codeCanvas.deleteSelected();
        }
    }

    function persist() {
        var dataToPersist = codeCanvas.serialise();
        console.log("Data to persist:", JSON.stringify(dataToPersist));
    }
}
