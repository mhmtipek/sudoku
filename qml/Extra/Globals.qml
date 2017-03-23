pragma Singleton

import QtQuick 2.0
import QtQuick.Window 2.0

QtObject {
    readonly property ListModel difficultyModel: ListModel {
        ListElement {
            name: qsTr("Hardest")
            difficulty: 4
        }
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

    function getDifficultyText(difficulty) {
        if (difficulty === 1)
            return qsTr("Easy");
        if (difficulty === 2)
            return qsTr("Normal");
        if (difficulty === 3)
            return qsTr("Hard");
        if (difficulty === 4)
            return qsTr("Hardest");
    }

    property QtObject style: QtObject {
        property QtObject colorPalette: QtObject {
            property color textColor: "#0d3832"
            property color alternateTextColor: "#60837e"
            property color linkColor: "#bd3f1d"
            property color titleColor: "#0d3832"
            property color lightTextColor: "#dedede"
            property color backgroundColor: "#f3e1c9"
            property color backgroundTextColor: "#93b3ae"
            property color borderColor: "#00796B"
            property color lineColor: "#176258"
            property color shadowColor: "#353535"
            property color buttonColor: "#f3e1c9"
            property color buttonColorPressed: "#f3d5b0"
            property color buttonBorderColor: "#176258"
            property color darkButtonColor: "#474747"
            property color darkButtonColorPressed: "#396d62"
            property color darkButtonBorderColor: "#000000"
            property color keyboardColor: "#f3e1c9"
            property color keyboardBorderColor: "#176258"
            property color boardColor: "#176258"
            property color cellColor: "#f3d5b0"
            property color cellColorAlternate: "#f3e1c9"
            property color sameRowCellColor: "#8dd6c9"
            property color sameColumnCellColor: "#b7e3dc"
            property color selectedCellColor: "#67cbb6"
            property color cellBorderColor: "#176258"
            property color editableCellTextColor: "#125147"
            property color uneditableCellTextColor: "#0d3832"
            property color sudokuGamePageBackground: "#E5A166"
            property color boardSlotColor: "#ebd3b6"
            property color boardSlotAlternateColor: "#e6d4bc"
        }

        property real shadowWidth: Screen.width * 0.015
    }
}
