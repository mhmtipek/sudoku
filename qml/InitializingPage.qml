import QtQuick 2.0
import QtGraphicalEffects 1.0
import "Extra"

Rectangle {
    id: initializingPage
    width: parent.width
    height: parent.height
    color: "#998ecfd5"
    visible: true

    signal finished()

    Component.onCompleted: {
        GameControl.creatingInitialTableFinished.connect(function() {
            visible = false;
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


