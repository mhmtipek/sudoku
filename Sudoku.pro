TEMPLATE = app

QT += qml quick widgets concurrent sql

SOURCES += main.cpp \
    abstractgamecontrol.cpp \
    gamecontrolproxy.cpp \
    gamecontrolfactory.cpp \
    standardsudokugamecontrol.cpp \
    standardsudokuinitialboardcreator.cpp \
    standardsudokusolver.cpp \
    standardsudokusearchtree.cpp \
    scoreboardmodel.cpp \
    sudokuboardlistmodelproxy.cpp \
    sudokuboardmodel.cpp \
    sudokuboardcelldata.cpp \
    utils.cpp \
    boardslotsmodel.cpp

RESOURCES += qml/qml.qrc \
    other.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    abstractgamecontrol.h \
    gamecontrolproxy.h \
    gamecontrolfactory.h \
    standardsudokugamecontrol.h \
    standardsudokuinitialboardcreator.h \
    standardsudokusolver.h \
    standardsudokusearchtree.h \
    utils.h \
    scoreboardmodel.h \
    sudokuboardlistmodelproxy.h \
    sudokuboardmodel.h \
    sudokuboardcelldata.h \
    boardslotsmodel.h

lupdate_only{
SOURCES += qml/AboutPage.qml \
   qml/BoardSlotsDialog.qml \
   qml/CircularButton.qml \
   qml/Dialog.qml \
   qml/Extra/Globals.qml \
   qml/GameTypePage.qml \
   qml/MainPage.qml \
   qml/RectangularButton.qml \
   qml/ScoreBoardPage.qml \
   qml/SliderView.qml \
   qml/StandardGameConfigurationPage.qml \
   qml/SudokuBoard.qml \
   qml/SudokuGamePage.qml \
   qml/SudokuKeyboard.qml \
   qml/WaitDialog.qml \
   qml/main.qml
}

TRANSLATIONS += sudoku_tr.ts

QMAKE_CXXFLAGS += -std=c++11

