#include "boardslotsmodel.h"

#include <QSqlRecord>
#include <QSqlField>

BoardSlotsModel::BoardSlotsModel(QObject *parent)
    : QSqlTableModel(parent)
{
    setTable("board_slots");
    refresh();
}

QHash<int, QByteArray> BoardSlotsModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[DateTimeRole] = "dateTime";
    names[DifficultyRole] = "difficulty";
    names[OccupancyPercentageRole] = "occupancyPercentage";
    names[IsEmptyRole] = "isEmpty";

    return names;
}

QVariant BoardSlotsModel::data(const QModelIndex &index, int role) const
{
    if (role == DateTimeRole)
        return QSqlTableModel::data(createIndex(index.row(), 4), Qt::DisplayRole);

    if (role == DifficultyRole)
        return QSqlTableModel::data(createIndex(index.row(), 5), Qt::DisplayRole);

    if (role == OccupancyPercentageRole)
        return QSqlTableModel::data(createIndex(index.row(), 6), Qt::DisplayRole);

    if (role == IsEmptyRole)
        return QVariant::fromValue(QSqlTableModel::data(createIndex(index.row(), 1), Qt::DisplayRole).toString() == "");

    return QVariant();
}

void BoardSlotsModel::refresh()
{
    setSort(0, Qt::AscendingOrder);
    QSqlTableModel::select();
}
