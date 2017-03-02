import QtQuick 2.0
import QtGraphicalEffects 1.0
import "Extra"

Item {
    id: root

    width: root.width * 0.7
    height: root.height * 0.1

    property string text

    signal clicked()

    Rectangle {
        id: buttonBackground

        anchors.fill: parent
        anchors.margins: Globals.style.shadowWidth

        color: buttonMouseArea.pressed ? Globals.style.colorPalette.buttonColorPressed
                                       : Globals.style.colorPalette.buttonColor

        radius: height * 0.2
    }

    DropShadow {
        anchors.fill: buttonBackground

        fast: false
        cached: false
        color: Globals.style.colorPalette.shadowColor
        radius: buttonMouseArea.pressed ? Globals.style.shadowWidth * 0.7 : Globals.style.shadowWidth
        source: buttonBackground
        spread: 0.2
        samples: 32
    }

    Text {
        anchors.centerIn: parent

        text: root.text
        font.pointSize: 24
        font.bold: true
        color: Globals.style.colorPalette.textColor
    }

    MouseArea {
        id: buttonMouseArea

        anchors.fill: parent

        onClicked: {
            root.clicked();
        }
    }
}
