#ifndef CELLDATA_H
#define CELLDATA_H

#include <QObject>

#define EMPTY_CELL_VALUE -1

/*!
 * \brief Contains cell data used in QML Grid view
 */
class SudokuBoardCellData : public QObject
{
    Q_OBJECT

    //! Value of cell
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)

    //! Status of cell.
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

    //! If value of cell is in conflict with another cell.
    Q_PROPERTY(bool inConflict READ inConflict WRITE setInConflict NOTIFY inConflictChanged)

    //! If cell is editable
    Q_PROPERTY(bool isEditable READ isEditable WRITE setEditable NOTIFY isEditableChanged)

public:
    SudokuBoardCellData(QObject *parent = 0);
    SudokuBoardCellData(int value, QObject *parent = 0);

    int value() const;
    void setValue(int value);

    QString status() const;
    void setStatus(const QString &status);

    bool inConflict() const;
    void setInConflict(bool ok);

    bool isEditable() const;
    void setEditable(bool ok);

    operator int() const;

signals:
    void valueChanged();
    void statusChanged();
    void inConflictChanged();
    void isEditableChanged();

private:
    int m_value;
    QString m_status;
    bool m_inConflict;
    bool m_isEditable;
};

//! Comparision with another SudokuBoardCellData object
bool operator ==(const SudokuBoardCellData &lhs, const SudokuBoardCellData &rhs);

//! Comparision with integer value. Returns true if cell value is equal to value, else returns false
bool operator ==(const SudokuBoardCellData &lhs, int value);

#endif // CELLDATA_H
