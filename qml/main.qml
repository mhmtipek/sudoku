import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import "Extra" 1.0

ApplicationWindow {
    id: applicationWindow

    title: qsTr("Sudoku")
    visible: true

    // Screen ratio is 9-16
    width: 450
    height: 800

    StackView {
        id: stackView

        anchors.fill: parent

        focus: true

        MainPage {
            id: mainPage

            visible: false

            onNewGameRequested: {
                if (GameControl.setGameType("standard")) {
                    stackView.push(standardGameConfigurationPage);
                    changeActiveFocusTimer.start();
                }
            }

            onScoreboardPageRequested: {
                GameControl.scoreBoardModel.refresh();
                stackView.push(scoreBoardPage);
                changeActiveFocusTimer.start();
            }

            onAboutPageRequested: {
                stackView.push(aboutPage);
                changeActiveFocusTimer.start();
            }

            onGameLoaded: {
                stackView.push(sudokuGamePage);
                changeActiveFocusTimer.start();
            }
        }

        SudokuGamePage {
            id: sudokuGamePage
            visible: false

            onBackRequested: {
                stackView.pop();
                changeActiveFocusTimer.start();
            }
        }

        GameTypePage {
            id: gameTypePage
            visible: false

            onGameRequested: {
                if (!GameControl.setGameType(type)) {
                    console.log("failed to set game control");
                    return;
                }

                if (type === "standard") {
                    stackView.push({item: standardGameConfigurationPage, replace: true});
                    changeActiveFocusTimer.start();
                }
            }
        }

        StandardGameConfigurationPage {
            id: standardGameConfigurationPage

            visible: false

            onFinished: {
                stackView.push({item: sudokuGamePage, replace: true});
                GameControl.createInitialTable();
                sudokuGamePage.clear();
                GameControl.start();
                changeActiveFocusTimer.start()
            }

            onBackRequested: {
                stackView.pop();
                changeActiveFocusTimer.start();
            }
        }

        ScoreBoardPage {
            id: scoreBoardPage
            visible: false

            onBackRequested: {
                stackView.pop();
                changeActiveFocusTimer.start()
            }
        }

        AboutPage {
            id: aboutPage
            visible: false

            onBackRequested: {
                stackView.pop();
                changeActiveFocusTimer.start()
            }
        }

        initialItem: mainPage

        Component.onCompleted: changeActiveFocusTimer.start()
    }

    Timer {
        id: changeActiveFocusTimer

        repeat: false
        interval: 200
        triggeredOnStart: false

        onTriggered: {
            stackView.currentItem.forceActiveFocus();
        }
    }

    property QtObject materialStyle: QtObject {
        property QtObject colorPalette: QtObject {
            property color textColor: "#454545"
            property color titleColor: "#454545"
            property color lightTextColor: "#DEDEDE"
            property color backgroundColor: "#F0F4C3"
            property color borderColor: "#134713"
            property color lineColor: "#388E3C"
            property color shadowColor: "#000000"
            property color buttonColor: "#C8E6C9"
            property color buttonColorPressed: "#B2EBF2"
            property color buttonBorderColor: "#C8E6C9"
            property color darkButtonColor: "#474747"
            property color darkButtonColorPressed: "#396d62"
            property color darkButtonBorderColor: "#000000"
            property color keyboardColor: "#757575"
            property color keyboardBorderColor: "#176258"
            property color boardColor: "#533200"
            property color cellColor: "#FFF5E5"
            property color cellColorAlternate: "#FFEED3"
            property color sameRowCellColor: "#CACE61"
            property color sameColumnCellColor: "#CACE61"
            property color selectedCellColor: "#AC517E"
            property color cellBorderColor: "#533200"
            property color editableCellTextColor: "#454545"
            property color uneditableCellTextColor: "#656565"
            property color sudokuGamePageBackground: "#11384F"
        }
    }
}
