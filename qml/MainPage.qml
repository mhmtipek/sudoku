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
    signal gameLoaded()
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
                ListElement {
                    label: qsTr("Continue last game")
                    actionId: 1
                }
                ListElement {
                    label: qsTr("New game")
                    actionId: 2
                }
                ListElement {
                    label: qsTr("Load game")
                    actionId: 3
                }
                ListElement {
                    label: qsTr("Scoreboard")
                    actionId: 4
                }
            }

            delegate: RectangularButton {
                width: root.width * 0.7
                height: root.height * 0.1

                visible: index === 0 ? (GameControl.previousGameUnfinished ? true : false) : true

                text: model.label

                onClicked: {
                    if (actionId === 1) {
                        GameControl.continueLastGame();
                        gameLoaded();
                    } else if (actionId === 2) {
                        root.newGameRequested();
                    } else if (actionId === 3) {
                        GameControl.boardSlotsModel.refresh();
                        boardSlotsDialog.show();
                    } else if (actionId === 4) {
                        root.scoreboardPageRequested();
                    }
                }
            }
        }
    }

    CircularButton {
        anchors.left: parent.left
        anchors.leftMargin: root.width * 0.05
        anchors.bottom: parent.bottom
        anchors.bottomMargin: anchors.leftMargin

        width: root.width * 0.18
        height: width

        imageUrl: "qrc:/info.png"

        onClicked: {
            root.aboutPageRequested();
        }
    }

    BoardSlotsDialog {
        id: boardSlotsDialog

        visible: false

        onSelected: {
            if (isEmpty)
                return;

            if (!GameControl.loadSavedGame(id)) {
                messageDialog.title = qsTr("Error");
                messageDialog.text = qsTr("Failed to load game");
                messageDialog.show();
                return;
            }

            boardSlotsDialog.hide();

            gameLoaded();
        }
    }

    Dialog {
        id: messageDialog

        buttons: [
            {
                imageUrl: "qrc:/cancel_white.png",
                onClicked: function() {
                    messageDialog.hide();
                }
            }
        ]
    }

    Keys.onBackPressed: {
        if (messageDialog.visible) {
            messageDialog.hide();
            return;
        }

        if (boardSlotsDialog.visible) {
            boardSlotsDialog.hide();
            return;
        }

        Qt.quit();
    }
}
