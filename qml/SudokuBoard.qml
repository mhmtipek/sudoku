import QtQuick 2.3
import QtQuick.Controls 1.2
import "Extra" 1.0

Rectangle {
    id: board

    signal cellClicked(bool isEditable)

    property real spacing: width * 0.006
    property real spacing3x3: width * 0.004

    color: Globals.style.colorPalette.boardColor

    function setCurrentCellValue(value) {
        GameControl.model.setCellValue(currentIndex, value);
    }

    function clearCurrentCellValue() {
        GameControl.model.clearCellValue(currentIndex);
    }

    function clearSelection() {
        GameControl.model.clearSelection();
    }

    property int currentIndex: -1

    function getCellColor(status, inConflict, index) {
        if (inConflict)
            return "red";

        if (status === "selected")
            return Globals.style.colorPalette.selectedCellColor;
        else if (status === "same_row")
            return Globals.style.colorPalette.sameRowCellColor;
        else if (status === "same_column")
            return Globals.style.colorPalette.sameColumnCellColor;

        var row = Math.floor(index / 9);
        var col = index % 9;

        var quadraticIndex = Math.floor(row / 3) * 3 + Math.floor(col / 3);

        return quadraticIndex % 2 === 0 ? Globals.style.colorPalette.cellColor
                                        : Globals.style.colorPalette.cellColorAlternate;
    }

    Grid {
        id: grid

        anchors.centerIn: parent

        columns: 9
        rows: 9

        Repeater {
            model: GameControl.model

            Item {
                width: index % 9 === 2 || index % 9 === 5 ? ((board.width - 2 * (board.spacing3x3 + board.spacing)) / grid.columns) + board.spacing3x3
                                                          : ((board.width - 2 * (board.spacing3x3 + board.spacing)) / grid.columns)
                height: Math.floor(index / 9) === 2 || Math.floor(index / 9) === 5 ? ((board.height - 2 * (board.spacing3x3 + board.spacing)) / grid.rows) + board.spacing3x3
                                                                                   : ((board.height - 2 * (board.spacing3x3 + board.spacing)) / grid.rows)

                clip: true

                Rectangle {
                    width: (board.width - 2 * (board.spacing3x3 + board.spacing)) / grid.columns
                    height: (board.height - 2 * (board.spacing3x3 + board.spacing)) / grid.rows

                    color: getCellColor(model.status, model.inConflict, index)
                    radius: board.spacing * 2
                    border.width: board.spacing / 2
                    border.color: Globals.style.colorPalette.cellBorderColor

                    Text {
                        anchors.centerIn: parent

                        text: model.value !== undefined && model.value !== -1 ? model.value : ""

                        font.pointSize: 20
                        font.weight: model.isEditable ? Font.DemiBold : Font.Black
                        color: model.isEditable ? Globals.style.colorPalette.editableCellTextColor : Globals.style.colorPalette.uneditableCellTextColor
                    }

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            board.cellClicked(model.isEditable);
                            board.currentIndex = model.index;
                            model.status = "selected";
                        }
                    }
                }
            }
        }
    }
}
