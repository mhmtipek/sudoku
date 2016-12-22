#ifndef SUDOKUBOARDMODEL_H
#define SUDOKUBOARDMODEL_H

#include <QAbstractTableModel>
#include <array>

#include "sudokuboardcelldata.h"

/*!
 * \brief Model of sudoku board
 *
 * This model is a 81x81 table model
 */
class SudokuBoardModel : public QAbstractTableModel
{
    Q_OBJECT

    friend class StandardSudokuGameControl;
    
public:
    /*!
     * \brief Cell properties are defined in this enum
     */
    enum CellRoles {
        ValueRole = Qt::UserRole + 1, //! Value of cell
        StatusRole, //! Status of cell
        ConflictRole, //! If cell is in conflict with another cell
        EditableRole //! If cell is editable
    };

    SudokuBoardModel(QObject *parent = 0);

    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex & index) const;
    QHash<int, QByteArray> roleNames() const;

    void clear();

    /*!
     * \brief Makes empty cells editable
     *
     * Used after initial board is created. Initial cell values are not editable
     * but empty cells are editable
     */
    void makeEmptyCellsEditable();

    /*!
     * \brief Makes all cells uneditable
     *
     * This function is called when game is finished to prevent user editing finished board
     *
     * \sa StandardSudokuGameControl
     */
    void makeAllCellsUneditable();

protected:
    bool setData(int row, int column, int value);

private:
    bool updateConflicts();

    std::array<std::array<SudokuBoardCellData, 9>, 9> m_dataGrid;

};

#endif // SUDOKUBOARDMODEL_H
