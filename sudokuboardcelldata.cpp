#include "sudokuboardcelldata.h"

#include <QMetaType>

bool operator ==(const SudokuBoardCellData &lhs, const SudokuBoardCellData &rhs)
{
    return lhs.value() == rhs.value();
}

bool operator ==(const SudokuBoardCellData &lhs, int value)
{
    return lhs.value() == value;
}

SudokuBoardCellData::SudokuBoardCellData(QObject *parent) :
    SudokuBoardCellData(EMPTY_CELL_VALUE, parent)
{

}

SudokuBoardCellData::SudokuBoardCellData(int value, QObject *parent) :
    QObject(parent),
    m_value(value),
    m_status("none"),
    m_inConflict(false),
    m_isEditable(false)
{

}

int SudokuBoardCellData::value() const
{
    return m_value;
}

void SudokuBoardCellData::setValue(int value)
{
    if ((value < 1 || value > 9) && value != EMPTY_CELL_VALUE)
        return;

    m_value = value;
    emit valueChanged();
}

QString SudokuBoardCellData::status() const
{
    return m_status;
}

void SudokuBoardCellData::setStatus(const QString &status)
{
    m_status = status;
}

bool SudokuBoardCellData::inConflict() const
{
    return m_inConflict;
}

void SudokuBoardCellData::setInConflict(bool ok)
{
    m_inConflict = ok;
    emit inConflictChanged();
}

bool SudokuBoardCellData::isEditable() const
{
    return m_isEditable;
}

void SudokuBoardCellData::setEditable(bool ok)
{
    m_isEditable = ok;
    emit isEditableChanged();
}

SudokuBoardCellData::operator int() const
{
    return m_value;
}
