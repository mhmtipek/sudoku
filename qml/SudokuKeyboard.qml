import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 1.2
import "Extra" 1.0

Rectangle {
    id: root

    radius: width * 0.02

    border.width: width * 0.006
    border.color: Globals.style.colorPalette.keyboardBorderColor

    color: Globals.style.colorPalette.keyboardColor

    function show() {
        visible = true;
    }

    function hide() {
        visible = false;
    }

    signal buttonClicked(int value)
    signal clearClicked

    Row {
        id: numbersRow

        anchors.centerIn: parent

        property int numberMargins: root.height * 0.14 - root.border.width

        spacing: numberMargins - root.border.width

        Repeater {
            model: 10

            delegate: Rectangle {
                height: root.height - 2 * numbersRow.numberMargins
                width: (root.width - 13 * (numbersRow.numberMargins - root.border.width)) / 10

                radius: width * 0.1

                color: mouseArea.pressed ? Globals.style.colorPalette.darkButtonColorPressed
                                         : Globals.style.colorPalette.darkButtonColor

                Image {
                    anchors.fill: parent
                    anchors.leftMargin: parent.width * 0.1
                    anchors.rightMargin: parent.width * 0.1

                    visible: index === 9
                    fillMode: Image.PreserveAspectFit
                    source: "qrc:/erase.png"
                    mipmap: true
                }

                Text {
                    anchors.centerIn: parent

                    visible: index !== 9
                    text: index === 9 ? "x" : index + 1
                    color: Globals.style.colorPalette.lightTextColor
                    font.pointSize: 18
                    font.weight: Font.Black
                }

                Image {
                    anchors.fill: parent

                    fillMode: Image.PreserveAspectFit
                }

                MouseArea {
                    id: mouseArea

                    anchors.fill: parent

                    onClicked: {
                        if (index === 9)
                            clearClicked();
                        else
                            buttonClicked(index + 1);
                    }
                }
            }
        }
    }
}

