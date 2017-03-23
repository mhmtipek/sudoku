#ifndef SUDOKUBOARDLISTMODEL_H
#define SUDOKUBOARDLISTMODEL_H

#include <QAbstractProxyModel>

#include <QUndoStack>

/*!
 * \brief Proxy model for SudokuBoardModel
 *
 * This model converts 81x81 table model to list model. QML view components
 * does not support table models
 *
 */
class SudokuBoardListModelProxy : public QAbstractProxyModel
{
    Q_OBJECT

    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)

    friend class GameControlProxy;

public:
    SudokuBoardListModelProxy(QObject *parent = 0);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QModelIndex mapFromSource(const QModelIndex & sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex & proxyIndex) const;

    void setSourceModel(QAbstractItemModel * sourceModel);

    virtual Qt::ItemFlags flags(const QModelIndex & index) const;
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void setCellValue(int index, int value);
    Q_INVOKABLE void clearCellValue(int index);

    Q_INVOKABLE void setCellStatus(int index, QString value);
    Q_INVOKABLE void clearSelection();

    Q_INVOKABLE void undo();
    bool canUndo() const;

    void clear();

signals:
    void canUndoChanged();

private slots:
    void emitDataChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles);

private:
    QUndoStack m_undoStack;

};

#endif // SUDOKUBOARDLISTMODEL_H
