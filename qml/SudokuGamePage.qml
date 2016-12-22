import QtQuick 2.3
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0
import "Extra" 1.0

Rectangle {
    id: root

    width: parent.width
    height: parent.height

    signal backRequested

    property real shadowWidth: width * 0.018

    property bool gameFinished: false

    Image {
        id: backgroundImage

        anchors.fill: parent

        fillMode: Image.PreserveAspectFit
        source: "qrc:/game_background.png"
        opacity: 0.06
    }

    color: Globals.style.colorPalette.sudokuGamePageBackground

    function clear() {
        confirmExitDialog.visible = false;
        gameFinishedDialog.visible = false;
        keyboard.hide();
    }

    Component.onCompleted: {
        GameControl.gameFinished.connect(function() {
            gameFinished = true;
            keyboard.hide();
            gameFinishedDialog.show();
        });
    }

    function fixTime(milliseconds) {
        var minutes = Math.floor(milliseconds / 60000);
        var seconds = Math.floor((milliseconds % 60000) / 1000);

        if (minutes !== 0)
            return qsTr("%1m %2s").arg(minutes).arg(seconds);
        else
            return qsTr("%1s").arg(seconds)
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            board.clearSelection();
            keyboard.hide();
        }
    }

    Item {
        id: header
        width: parent.width
        height: parent.height * 0.15

        CircularButton {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin:  (parent.height - height) / 3

            width: root.width * 0.2
            height: width

            imageUrl: "qrc:/back.png"

            onClicked: {
                if (GameControl.finishTime() > 0)
                    backRequested();
                else
                    confirmExitDialog.show();
            }
        }

        Item {
            id: undoButtonContainer

            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: (parent.height - height) / 3

            width: root.width * 0.2
            height: width

            Rectangle {
                id: undoButton

                anchors.fill: parent
                anchors.margins: root.shadowWidth * 2

                color: undoButton_MouseArea.pressed ? Globals.style.colorPalette.buttonColorPressed
                                                    : Globals.style.colorPalette.buttonColor
                radius: width / 2
                border.width: width * 0.05
                border.color: Globals.style.colorPalette.buttonBorderColor
                visible: GameControl.model.canUndo

                Image {
                    anchors.fill: parent
                    anchors.margins: parent.width * 0.1

                    source: "qrc:/undo.png"
                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                }

                MouseArea {
                    id: undoButton_MouseArea
                    anchors.fill: parent
                    onClicked: {
                        GameControl.model.undo();
                    }
                }
            }
        }

        DropShadow {
            anchors.fill: undoButtonContainer

            fast: true
            cached: true
            color: Globals.style.colorPalette.shadowColor
            radius: undoButton_MouseArea.pressed ? root.shadowWidth * 0.7 : root.shadowWidth
            source: undoButtonContainer
            spread: 0.2
            samples: 32
        }
    }

    Item {
        id: boardContainer
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        height: width + 2 * root.shadowWidth

        SudokuBoard {
            id: board

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            height: width

            onCellClicked: {
                if (isEditable)
                    keyboard.show();
                else
                    keyboard.hide();
            }
        }
    }

    DropShadow {
        anchors.fill: boardContainer

        fast: true
        cached: true
        color: Globals.style.colorPalette.shadowColor
        source: boardContainer
        spread: 0.2
        radius: root.shadowWidth
        samples: 32
    }

    Item {
        id: keyboardContainer

        anchors.top: boardContainer.bottom
        anchors.topMargin: root.height * 0.02
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        SudokuKeyboard {
            id: keyboard

            anchors.top: parent.top
            anchors.topMargin: root.shadowWidth
            anchors.horizontalCenter: parent.horizontalCenter

            height: root.height * 0.09
            width: root.width * 0.9

            visible: false

            onButtonClicked: {
                board.setCurrentCellValue(value);
            }

            onClearClicked: {
                board.clearCurrentCellValue();
            }

            onVisibleChanged: {
                board.clearSelection();
            }
        }
    }

    DropShadow {
        anchors.fill: keyboardContainer

        fast: true
        cached: true
        color: Globals.style.colorPalette.shadowColor
        source: keyboardContainer
        spread: 0.2
        radius: root.shadowWidth
        samples: 32
    }

    Dialog {
        id: gameFinishedDialog

        title: qsTr("Congrats!")

        body: Item {
            id: gameFinishedDialog_body

            width: root.width * 0.7
            height: root.height * 0.3

            Text {
                id: gameFinishedDialog_text
                anchors.left: parent.left
                anchors.leftMargin: parent.width * 0.08
                anchors.right: parent.right
                anchors.rightMargin: parent.width * 0.08
                anchors.top: parent.top
                anchors.topMargin: parent.height * 0.08

                text: qsTr("Game is finished!")
                font.pointSize: 22
                wrapMode: Text.WordWrap
                elide: Text.ElideRight
                color: Globals.style.colorPalette.textColor
            }

            Row {
                anchors.left: parent.left
                anchors.leftMargin: parent.width * 0.08
                anchors.top: gameFinishedDialog_text.bottom
                anchors.topMargin: parent.height * 0.08

                spacing: gameFinishedDialog_body.width * 0.05

                Image {
                    height: gameFinishedDialog_body.height * 0.2
                    width: height

                    source: "qrc:/stopwatch.png"
                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                }

                Text {
                    height: gameFinishedDialog_body.height * 0.2

                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 22
                    text: gameFinished ? fixTime(GameControl.finishTime()) : ""
                    color: Globals.style.colorPalette.textColor
                }
            }
        }
    }

    Dialog {
        id: confirmExitDialog

        title: qsTr("Confirm")
        text: qsTr("Game is not finished. Are you sure to exit?");

        buttons: [
            {
                imageUrl: "qrc:/cancel_white.png",
                onClicked: function() {
                    confirmExitDialog.hide();
                }
            },
            {
                imageUrl: "qrc:/check_white.png",
                onClicked: function() {
                    root.backRequested();
                }
            }
        ]
    }

    Keys.onBackPressed: {
        if (gameFinishedDialog.visible) {
            gameFinishedDialog.hide();
            return;
        }

        if (confirmExitDialog.visible) {
            confirmExitDialog.hide();
            return;
        }

        if (GameControl.finishTime() > 0)
            backRequested();
        else
            confirmExitDialog.show();
    }
}
