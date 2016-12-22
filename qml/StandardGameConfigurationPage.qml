import QtQuick 2.0

Rectangle {
    id: root

    color: Qt.lighter("orange")

    signal finished()
    signal backRequested()

    Image {
        anchors.fill: parent

        source: "qrc:/game_background.png"
        opacity: 0.18
    }

    CircularButton {
        id: backButton

        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0.05
        anchors.top: parent.top
        anchors.topMargin: anchors.leftMargin

        width: root.width * 0.2
        height: width

        imageUrl: "qrc:/back.png"

        onClicked: {
            root.backRequested();
        }
    }

    Column {
        id: buttonsColumn

        anchors.centerIn: parent

        Repeater {
            model: ListModel {
                ListElement {
                    name: qsTr("Hard")
                    difficulty: 3
                }
                ListElement {
                    name: qsTr("Normal")
                    difficulty: 2
                }
                ListElement {
                    name: qsTr("Easy")
                    difficulty: 1
                }
            }

            delegate: RectangularButton {
                width: root.width * 0.7
                height: root.height * 0.1

                text: model.name

                onClicked: {
                    GameControl.setDifficulty(model.difficulty);
                    finished();
                }
            }
        }
    }

    Keys.onBackPressed: {
        root.backRequested();
    }
}
