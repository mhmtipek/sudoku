import QtQuick 2.0
import QtGraphicalEffects 1.0
import "Extra"

Dialog {
    id: root

    height: bodyItem.height + width * 0.2

    titleVisible: false

    signal selected(int id, bool isEmpty)

    function getDifficultyName(value) {
        for (var i = 0; i < Globals.difficultyModel.count; ++i) {
            if (Globals.difficultyModel.get(i).difficulty === value)
                return Globals.difficultyModel.get(i).name;
        }

        return "";
    }

    body: Column {
        spacing: root.height * 0.03

        Repeater {
            model: GameControl.boardSlotsModel

            delegate: Item {
                width: root.width
                height: width * 0.2

                Rectangle {
                    anchors.centerIn: parent

                    height: parent.height
                    width: parent.width * 0.9

                    radius: height * 0.1
                    color: mouseArea.pressed ? Globals.style.colorPalette.boardSlotAlternateColor
                                             : Globals.style.colorPalette.boardSlotColor

                    Text {
                        anchors.centerIn: parent

                        font.pointSize: 28
                        font.weight: Font.Bold
                        text: qsTr("Empty slot")
                        color: Globals.style.colorPalette.backgroundTextColor
                        visible: isEmpty
                    }

                    Column {
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width * 0.07
                        anchors.verticalCenter: parent.verticalCenter

                        visible: !isEmpty

                        Text {
                            text: getDifficultyName(difficulty)
                            color: Globals.style.colorPalette.textColor
                            font.pointSize: 22
                            font.weight: Font.Bold
                        }

                        Text {
                            text: dateTime
                            color: Globals.style.colorPalette.alternateTextColor
                            font.pointSize: 14
                        }
                    }

                    Text {
                        anchors.right: parent.right
                        anchors.rightMargin: parent.width * 0.07
                        anchors.verticalCenter: parent.verticalCenter

                        text: "%" + occupancyPercentage
                        color: Globals.style.colorPalette.alternateTextColor
                        font.pointSize: 20
                        font.weight: Font.Bold
                        visible: !isEmpty
                    }

                    MouseArea {
                        id: mouseArea

                        anchors.fill: parent

                        onClicked: {
                            root.selected(index, isEmpty);
                        }
                    }
                }
            }
        }
    }
}
