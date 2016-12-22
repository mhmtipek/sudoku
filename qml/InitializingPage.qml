import QtQuick 2.0
import "Extra"

Rectangle {
    id: root

    signal finished()

    color: "#8ecfd5"

    Component.onCompleted: {
        GameControl.creatingInitialTableFinished.connect(function() {
            finished();
        });
    }

    Text {
        anchors.centerIn: parent

        text: GameControl.initialTableCreationProgressText + "..."

        font.pointSize: 28
        color: Globals.style.colorPalette.textColor
        wrapMode: Text.WordWrap
    }
}

