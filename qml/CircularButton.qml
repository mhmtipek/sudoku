import QtQuick 2.0
import QtGraphicalEffects 1.0
import "Extra" 1.0

Item {
    id: root

    width: root.width * 0.2
    height: width

    property int shadowWidth: width * 0.08

    property string imageUrl

    property color color: Globals.style.colorPalette.buttonColor
    property color colorWhenPressed: Globals.style.colorPalette.buttonColorPressed
    property color borderColor: Globals.style.colorPalette.buttonBorderColor

    signal clicked()

    Rectangle {
        id: background

        anchors.fill: parent
        anchors.margins: root.shadowWidth * 2

        color: mouseArea.pressed ? root.colorWhenPressed : root.color
        radius: width / 2
        border.width: width * 0.05
        border.color: root.borderColor

        Image {
            anchors.fill: parent
            anchors.margins: parent.width * 0.2

            source: root.imageUrl
            fillMode: Image.PreserveAspectFit
            smooth: true
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: {
                root.clicked();
            }
        }
    }

    DropShadow {
        anchors.fill: background

        fast: false
        cached: false
        color: Globals.style.colorPalette.shadowColor
        radius: mouseArea.pressed ? root.shadowWidth * 0.7 : root.shadowWidth
        source: background
        spread: 0.2
        samples: 32
    }
}


