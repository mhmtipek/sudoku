import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import "Extra"

Rectangle {
    id: root

    width: parent.width
    height: parent.height

    signal newGameRequested()
    signal scoreboardPageRequested()
    signal aboutPageRequested()

    color: "lightgreen"

    Image {
        anchors.fill: parent

        source: "qrc:/game_background.png"
        opacity: 0.18
    }

    Column {
        id: buttonsColumn

        anchors.centerIn: parent

        Repeater {
            model: ListModel {
                ListElement { label: qsTr("New game") }
                ListElement { label: qsTr("Scoreboard") }
            }

            delegate: RectangularButton {
                width: root.width * 0.7
                height: root.height * 0.1

                text: model.label

                onClicked: {
                    if (index === 0)
                        root.newGameRequested();
                    else if (index === 1)
                        root.scoreboardPageRequested();
                }
            }
        }
    }

    CircularButton {
        anchors.left: parent.left
        anchors.leftMargin: root.width * 0.05
        anchors.bottom: parent.bottom
        anchors.bottomMargin: anchors.leftMargin

        width: root.width * 0.2
        height: width

        imageUrl: "qrc:/info.png"

        onClicked: {
            root.aboutPageRequested();
        }
    }
}
