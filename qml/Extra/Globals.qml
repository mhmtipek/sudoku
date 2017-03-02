pragma Singleton

import QtQuick 2.0
import QtQuick.Window 2.0

QtObject {
    property QtObject style: QtObject {
        property QtObject colorPalette: QtObject {
            property color textColor: "#0d3832"
            property color linkColor: "#bd3f1d"
            property color titleColor: "#0d3832"
            property color lightTextColor: "#dedede"
            property color backgroundColor: "#f3e1c9"
            property color backgroundTextColor: "#90a9a5"
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
        }

        property real shadowWidth: Screen.width * 0.015
    }
}
