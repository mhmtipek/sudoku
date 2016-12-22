#include "sudokuboardcontrol.h"

#include "sudokuboardlinearmodel.h"

SudokuBoardControl::SudokuBoardControl(SudokuBoardLinearModel *sudokuBoardLinearModel, QObject *parent) :
    QObject(parent),
    m_sudokuBoardLinearModel(sudokuBoardLinearModel)
{
    // Should this must be done this way?
    connect(m_sudokuBoardLinearModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(checkForFinished()));
}

void SudokuBoardControl::checkForFinished()
{
    // TODO: Implement
}
