#ifndef SCOREBOARDMODEL_H
#define SCOREBOARDMODEL_H

#include <QSqlTableModel>

/*!
 * \brief Table model for scoreboard table in SQLITE db.
 *
 * Filters sudokuboard table according to given properties.
 */
class ScoreBoardModel : public QSqlTableModel
{
    Q_OBJECT

    Q_PROPERTY(QString gameType READ gameType WRITE setGameType NOTIFY gameTypeChanged)
    Q_PROPERTY(int difficulty READ difficulty WRITE setDifficulty NOTIFY difficultyChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int average READ average NOTIFY averageChanged)

public:
    //! Roles to be used in QML files
    enum Roles {
        GameTypeRole = Qt::UserRole + 1,
        DifficultyRole,
        PlayerNameRole,
        ScoreRole
    };

    ScoreBoardModel(QObject *parent = 0);

    //! Needed for QML
    QHash<int, QByteArray> roleNames() const;

    //! Returns data of given role at index
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    //! Returns current game type
    QString gameType() const;
    //! Sets game type
    void setGameType(QString type);

    //! Returns difficulty
    int difficulty() const;
    /*!
     * \brief Sets difficulty. Provided value must be in [1,4]
     * \param value Value to
     */
    void setDifficulty(int value);

    //! Returns number of scores
    int count() const;

    //! Returns average of selected difficulty in milliseconds
    int average() const;

    //! Fetches records from SQLITE database
    Q_INVOKABLE void refresh();

signals:
    void gameTypeChanged();
    void difficultyChanged();
    void countChanged();
    void averageChanged();

private:
    QString m_gameType;
    int m_difficulty;
    int m_average;
};

#endif // SCOREBOARDMODEL_H
