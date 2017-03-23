#ifndef BOARDSLOTSMODEL_H
#define BOARDSLOTSMODEL_H

#include <QSqlTableModel>

/*!
 * \brief Table model for board_slots table in SQLITE db.
 *
 */
class BoardSlotsModel : public QSqlTableModel
{
    Q_OBJECT

public:
    //! Roles to be used in QML files
    enum Roles {
        DateTimeRole = Qt::UserRole + 1,
        DifficultyRole,
        OccupancyPercentageRole,
        IsEmptyRole
    };

    BoardSlotsModel(QObject *parent = 0);

    //! Needed for QML
    QHash<int, QByteArray> roleNames() const;

    //! Returns data of given role at index
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    //! Fetches records from SQLITE database
    Q_INVOKABLE void refresh();

};

#endif // BOARDSLOTSMODEL_H
