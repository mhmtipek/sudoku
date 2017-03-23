#include "abstractgamecontrol.h"

AbstractGameControl::AbstractGameControl(QObject *parent) :
    QObject(parent)
{

}

AbstractGameControl::~AbstractGameControl()
{

}

void AbstractGameControl::setModel(SudokuBoardModel *model)
{
    Q_UNUSED(model)
}

void AbstractGameControl::setElapsedTime(int duration)
{
    Q_UNUSED(duration)
}
