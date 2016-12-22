#include "sudokuboardlistmodelproxy.h"

#include "sudokuboardmodel.h"

#include <QUndoCommand>

class UndoCommand : public QUndoCommand
{
public:
    explicit UndoCommand(SudokuBoardListModelProxy *model, int index, int previousValue) :
        m_model(model),
        m_index(index),
        m_previousValue(previousValue)
    {

    }

    void undo() {
        m_model->setCellValue(m_index, m_previousValue);
    }

private:
    SudokuBoardListModelProxy *m_model;

    int m_index;
    int m_previousValue;
};

SudokuBoardListModelProxy::SudokuBoardListModelProxy(QObject *parent) :
    QAbstractProxyModel(parent)
{
}

QModelIndex SudokuBoardListModelProxy::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return createIndex(row, column);
}

QModelIndex SudokuBoardListModelProxy::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}

int SudokuBoardListModelProxy::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 81;
}

int SudokuBoardListModelProxy::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QModelIndex SudokuBoardListModelProxy::mapFromSource(const QModelIndex &sourceIndex) const
{
    return createIndex(sourceIndex.row() * 9 + sourceIndex.column(), 0);
}

QModelIndex SudokuBoardListModelProxy::mapToSource(const QModelIndex &proxyIndex) const
{
    return createIndex(proxyIndex.row() / 9, proxyIndex.row() % 9);
}

void SudokuBoardListModelProxy::setSourceModel(QAbstractItemModel *sourceModel)
{
    if (this->sourceModel() != nullptr) {
        disconnect(this->sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                   this, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
    }

    connect(sourceModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(emitDataChanged(QModelIndex,QModelIndex,QVector<int>)));

    m_undoStack.clear();
    emit canUndoChanged();

    QAbstractProxyModel::setSourceModel(sourceModel);
}

Qt::ItemFlags SudokuBoardListModelProxy::flags(const QModelIndex &index) const
{
    if (sourceModel() != 0)
        return sourceModel()->flags(index);

    return 0;
}

QHash<int, QByteArray> SudokuBoardListModelProxy::roleNames() const
{
    if (sourceModel() != nullptr)
        return sourceModel()->roleNames();

    return QHash<int, QByteArray>();
}

void SudokuBoardListModelProxy::setCellValue(int index, int value)
{
    const int currentValue = data(createIndex(index, 0), Qt::DisplayRole).toInt();

    if (value == currentValue)
        return;

    m_undoStack.push(new UndoCommand(this,
                                     index,
                                     currentValue));

    emit canUndoChanged();

    setData(createIndex(index, 0), value, Qt::DisplayRole);
}

void SudokuBoardListModelProxy::clearCellValue(int index)
{
    const int currentValue = data(createIndex(index, 0), Qt::DisplayRole).toInt();

    if (EMPTY_CELL_VALUE == currentValue)
        return;

    m_undoStack.push(new UndoCommand(this,
                                     index,
                                     currentValue));

    emit canUndoChanged();

    setData(createIndex(index, 0), EMPTY_CELL_VALUE, Qt::DisplayRole);
}

void SudokuBoardListModelProxy::setCellStatus(int index, QString value)
{
    setData(createIndex(index, 0), value, SudokuBoardModel::StatusRole);
}

void SudokuBoardListModelProxy::clearSelection()
{
    for (int x = 0; x < rowCount(); ++x)
        setCellStatus(x, "none");
}

void SudokuBoardListModelProxy::undo()
{
    m_undoStack.undo();
    emit canUndoChanged();
}

bool SudokuBoardListModelProxy::canUndo() const
{
    return m_undoStack.canUndo();
}

void SudokuBoardListModelProxy::clear()
{
    m_undoStack.clear();
    emit canUndoChanged();
}

void SudokuBoardListModelProxy::emitDataChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles)
{
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight), roles);
}
