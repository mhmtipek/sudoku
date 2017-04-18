#include "scoreboardmodel.h"

#include <QSqlRecord>
#include <QSqlField>

ScoreBoardModel::ScoreBoardModel(QObject *parent)
    : QSqlTableModel(parent),
      m_gameType("standard"),
      m_difficulty(1),
      m_average(-1)
{
    setTable("scoreboard");
    refresh();
}

QHash<int, QByteArray> ScoreBoardModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[GameTypeRole] = "gameType";
    names[DifficultyRole] = "difficulty";
    names[PlayerNameRole] = "playerName";
    names[ScoreRole] = "score";

    return names;
}

QVariant ScoreBoardModel::data(const QModelIndex &index, int role) const
{
    if (role == GameTypeRole)
        return QSqlTableModel::data(createIndex(index.row(), 0), Qt::DisplayRole);

    if (role == DifficultyRole)
        return QSqlTableModel::data(createIndex(index.row(), 1), Qt::DisplayRole);

    if (role == PlayerNameRole)
        return QSqlTableModel::data(createIndex(index.row(), 2), Qt::DisplayRole);

    if (role == ScoreRole)
        return QSqlTableModel::data(createIndex(index.row(), 3), Qt::DisplayRole);

    return QVariant();
}

QString ScoreBoardModel::gameType() const
{
    return m_gameType;
}

void ScoreBoardModel::setGameType(QString type)
{
    m_gameType = type;
    refresh();
    emit gameTypeChanged();
}

int ScoreBoardModel::difficulty() const
{
    return m_difficulty;
}

void ScoreBoardModel::setDifficulty(int value)
{
    m_difficulty = value;
    refresh();
    emit difficultyChanged();
}

int ScoreBoardModel::count() const
{
    return rowCount();
}

int ScoreBoardModel::average() const
{
    return m_average;
}

void ScoreBoardModel::refresh()
{
    setFilter(QString("difficulty=%1 and game_type='%2'").arg(m_difficulty).arg(m_gameType));
    setSort(3, Qt::AscendingOrder);
    QSqlTableModel::select();

    if (rowCount() > 0) {
        int total = 0;
        for (int i = 0; i < rowCount(); ++i)
            total += QSqlTableModel::data(createIndex(i, 3), Qt::DisplayRole).toInt();
        m_average = total / rowCount();
    } else {
        m_average = -1;
    }

    emit averageChanged();
    emit countChanged();
}
