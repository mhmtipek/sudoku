import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQml.Models 2.1
import QtGraphicalEffects 1.0
import "Extra" 1.0

Rectangle {
    id: root

    signal backRequested

    color: "#81d2c9"

    Image {
        id: backgroundImage

        anchors.fill: parent

        fillMode: Image.PreserveAspectFit
        source: "qrc:/game_background.png"
        opacity: 0.06
    }

    Keys.onBackPressed: {
        if (clearDataConfirmationDialog.visible) {
            clearDataConfirmationDialog.hide();
            return;
        }

        root.backRequested();
    }

    function getDifficultyText(difficulty) {
        if (difficulty === 1)
            return qsTr("Easy");

        if (difficulty === 2)
            return qsTr("Normal");

        if (difficulty === 3)
            return qsTr("Hard");

        if (difficulty === 4)
            return qsTr("Hardest");

        return "";
    }

    function fixTime(milliseconds) {
        var minutes = Math.floor(milliseconds / 60000);
        var seconds = Math.floor((milliseconds % 60000) / 1000);

        if (minutes !== 0)
            return qsTr("%1m %2s").arg(minutes).arg(seconds);
        else
            return qsTr("%1s").arg(seconds)
    }

    CircularButton {
        id: backButton

        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0.05
        anchors.top: parent.top
        anchors.topMargin: anchors.leftMargin

        width: root.width * 0.18
        height: width

        imageUrl: "qrc:/back.png"

        onClicked: {
            root.backRequested();
        }
    }

    CircularButton {
        id: clearDataButton

        anchors.right: parent.right
        anchors.rightMargin: parent.width * 0.05
        anchors.top: parent.top
        anchors.topMargin: anchors.rightMargin

        width: root.width * 0.18
        height: width

        imageUrl: "qrc:/reset.png"

        onClicked: {
            clearDataConfirmationDialog.show();
        }
    }

    Item {
        id: bodyItem

        anchors.top: backButton.bottom
        anchors.topMargin: backButton.anchors.topMargin
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: parent.width * 0.1

        Rectangle {
            id: bodyItemBackground

            anchors.fill: parent
            anchors.topMargin: Globals.style.shadowWidth
            anchors.bottomMargin: Globals.style.shadowWidth

            color: Globals.style.colorPalette.backgroundColor
        }
    }

    DropShadow {
        anchors.fill: bodyItem

        fast: true
        cached: true
        color: Globals.style.colorPalette.shadowColor
        source: bodyItem
        spread: 0.2
        radius: Globals.style.shadowWidth
        samples: 32
    }

    Item {
        anchors.fill: bodyItem
        anchors.topMargin: Globals.style.shadowWidth * 2
        anchors.bottomMargin: Globals.style.shadowWidth * 2

        Row {
            id: difficultyTabsRow

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top

            Repeater {
                model: 4

                delegate: Item {
                    width: root.width / 4
                    height: root.height * 0.1

                    Text {
                        anchors.centerIn: parent

                        font.pointSize: 22
                        font.weight: GameControl.scoreBoardModel.difficulty === index + 1 ? Font.Bold
                                                                                          : Font.Normal
                        text: getDifficultyText(index + 1)

                        color: Globals.style.colorPalette.titleColor
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            GameControl.scoreBoardModel.difficulty = index + 1;
                        }
                    }
                }
            }
        }

        Text {
            anchors.centerIn: parent

            text: qsTr("No score")

            font.pointSize: 36
            color: Globals.style.colorPalette.backgroundTextColor
            font.weight: Font.Bold
            visible: GameControl.scoreBoardModel.count === 0
        }

        ListView {
            anchors.top: difficultyTabsRow.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            width: root.width * 0.95

            visible: GameControl.scoreBoardModel.count > 0

            model: GameControl.scoreBoardModel
            delegate: scoresListDelegateComponent
            spacing: height * 0.01
            clip: true
        }
    }

    Component {
        id: scoresListDelegateComponent

        Rectangle {
            id: delegateRoot

            width: root.width * 0.95
            height: root.height * 0.07

            radius: width * 0.02
            color: "white"
            opacity: 0.5

            Text {
                anchors.left: parent.left
                anchors.leftMargin: parent.width * 0.05
                anchors.verticalCenter: parent.verticalCenter

                font.pointSize: 20
                font.weight: Font.Bold
                text: fixTime(model.score)

                color: "black"
            }
        }
    }

    Dialog {
        id: clearDataConfirmationDialog

        title: qsTr("Please confirm")
        text: qsTr("Are you sure to reset all scores?")

        buttons: [
            {
                imageUrl: "qrc:/cancel_white.png",
                onClicked: function() {
                    clearDataConfirmationDialog.hide();
                }
            },
            {
                imageUrl: "qrc:/check_white.png",
                onClicked: function() {
                    GameControl.clearScoreBoardData("standard");
                    clearDataConfirmationDialog.hide();
                }
            }
        ]
    }
}
