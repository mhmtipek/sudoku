import QtQuick 2.3
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0
import "Extra" 1.0

Rectangle {
    id: root

    width: parent.width
    height: parent.height

    signal backRequested

    property string finishTime: "-"

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
        gameSummaryContainer.visible = false;
        keyboard.hide();
    }

    function showGameSummaryContainer() {
        gameSummaryContainer_showAnimation.start();
        gameFinishedDialog.onVisibleChanged.disconnect(showGameSummaryContainer);
    }

    Component.onCompleted: {
        GameControl.gameFinished.connect(function() {
            keyboard.hide();
            gameFinishedDialog.show();
            finishTime = fixTime(GameControl.finishTime());
            gameFinishedDialog.onVisibleChanged.connect(showGameSummaryContainer);
        });

        GameControl.onInitialTableCreationProgressChanged.connect(function () {
            if (GameControl.initialTableCreationProgress === 100) {
                if (waitDialog.visible)
                    waitDialog.hide();
            } else {
                if (!waitDialog.visible)
                    waitDialog.show();
            }
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
            id: backButton

            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: (parent.height - height) / 2

            width: root.width * 0.18
            height: width

            imageUrl: "qrc:/back.png"

            onClicked: {
                if (GameControl.finishTime() > 0)
                    backRequested();
                else
                    confirmExitDialog.show();
            }
        }

        Row {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: (parent.height - backButton.height) / 2

            spacing: anchors.rightMargin - Globals.style.shadowWidth

            CircularButton {
                width: root.width * 0.18
                height: width

                imageUrl: "qrc:/undo.png"
                visible: GameControl.model.canUndo

                onClicked: {
                    GameControl.model.undo();
                }
            }

            CircularButton {
                width: root.width * 0.18
                height: width

                imageUrl: "qrc:/save.png"

                onClicked: {
                    boardSlotsDialog.show();
                }
            }
        }
    }

    Item {
        id: boardContainer
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        height: width + 2 * Globals.style.shadowWidth

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
        radius: Globals.style.shadowWidth
        samples: 32
    }

    Column {
        id: gameSummaryContainer

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: boardContainer.bottom
        anchors.topMargin: root.height * 0.04

        spacing: root.height * 0.015
        visible: false

        NumberAnimation {
            id: gameSummaryContainer_showAnimation

            target: gameSummaryContainer
            property: "opacity"
            from: 0
            to: 1
            duration: 400
            onStarted: {
                gameSummaryContainer.opacity = 0;
                gameSummaryContainer.visible = true;
            }
        }

        Row {
            spacing: root.width * 0.02

            Image {
                width: root.width * 0.1
                height: width

                source: "qrc:/stopwatch.png"
                fillMode: Image.PreserveAspectFit
            }

            Text {
                height: root.width * 0.1
                verticalAlignment: Text.AlignVCenter

                text: root.finishTime
                font.pointSize: 24
                font.weight: Font.Bold
                color: Globals.style.colorPalette.textColor
            }
        }

        Row {
            spacing: root.width * 0.02

            Image {
                width: root.width * 0.1
                height: width

                source: "qrc:/star.png"
                fillMode: Image.PreserveAspectFit
            }

            Text {
                height: root.width * 0.1
                verticalAlignment: Text.AlignVCenter

                text: "#" + GameControl.rank + " (" + Globals.getDifficultyText(GameControl.difficulty()) + ")"
                font.pointSize: 24
                font.weight: Font.Bold
                color: Globals.style.colorPalette.textColor
            }
        }
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
            anchors.topMargin: Globals.style.shadowWidth
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
        radius: Globals.style.shadowWidth
        samples: 32
    }

    Dialog {
        id: gameFinishedDialog

        title: qsTr("Congrats!")

        body: Item {
            id: gameFinishedDialog_body

            width: root.width * 0.7
            height: root.height * 0.27

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
                id: finishTimeRow

                anchors.left: parent.left
                anchors.leftMargin: parent.width * 0.08
                anchors.top: gameFinishedDialog_text.bottom
                anchors.topMargin: parent.height * 0.15

                spacing: gameFinishedDialog_body.width * 0.05

                Image {
                    height: gameFinishedDialog_body.height * 0.16
                    width: height

                    source: "qrc:/stopwatch.png"
                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                }

                Text {
                    height: gameFinishedDialog_body.height * 0.16

                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 22
                    text: root.finishTime
                    color: Globals.style.colorPalette.textColor
                }
            }

            Row {
                anchors.left: parent.left
                anchors.leftMargin: parent.width * 0.08
                anchors.top: finishTimeRow.bottom
                anchors.topMargin: parent.height * 0.08

                spacing: gameFinishedDialog_body.width * 0.05

                Image {
                    height: gameFinishedDialog_body.height * 0.16
                    width: height

                    source: "qrc:/star.png"

                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                }

                Text {
                    height: gameFinishedDialog_body.height * 0.16

                    anchors.verticalCenterOffset: height * 0.1
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 22
                    text: "#" + GameControl.rank + " (" + Globals.getDifficultyText(GameControl.difficulty()) + ")"
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
                    confirmExitDialog.hide();
                    root.backRequested();
                }
            }
        ]
    }

    BoardSlotsDialog {
        id: boardSlotsDialog

        visible: false

        onSelected: {
            if (isEmpty) {
                GameControl.saveBoardToSlot(id);
                boardSlotsDialog.hide();
            } else {
                confirmSaveDialog.slotId = id;
                confirmSaveDialog.show();
            }
        }
    }

    Dialog {
        id: confirmSaveDialog

        title: qsTr("Confirm")
        text: qsTr("This slot is not empty. Are you sure to replace?")

        property int slotId

        buttons: [
            {
                imageUrl: "qrc:/cancel_white.png",
                onClicked: function() {
                    confirmSaveDialog.hide();
                }
            },
            {
                imageUrl: "qrc:/check_white.png",
                onClicked: function() {
                    GameControl.saveBoardToSlot(confirmSaveDialog.slotId);
                    confirmSaveDialog.hide();
                }
            }
        ]
    }

    Keys.onBackPressed: {
        if (keyboard.visible) {
            board.clearSelection();
            keyboard.hide();
            return;
        }

        if (gameFinishedDialog.visible) {
            gameFinishedDialog.hide();
            return;
        }

        if (confirmExitDialog.visible) {
            confirmExitDialog.hide();
            return;
        }

        if (confirmSaveDialog.visible) {
            confirmSaveDialog.hide();
            return;
        }

        if (boardSlotsDialog.visible) {
            boardSlotsDialog.hide();
            return;
        }

        if (GameControl.finishTime() > 0)
            backRequested();
        else
            confirmExitDialog.show();
    }

    WaitDialog {
        id: waitDialog

        visible: false
        text: GameControl.initialTableCreationProgressText + "..."

        Component.onCompleted: {
            GameControl.creatingInitialTableFinished.connect(function() {
                waitDialog.hide();
            });
        }
    }
}
