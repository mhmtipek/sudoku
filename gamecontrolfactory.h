#ifndef GAMECONTROLFACTORY_H
#define GAMECONTROLFACTORY_H

#include "abstractgamecontrol.h"

#include <QStringList>

/*!
 * \brief Factory class for game controls
 *
 * When the project started, it is planned to implement
 * more than one sudoku types. Later decided to release with
 * only standard sudoku game and add other sudoku game types soon.
 * This class will be used later effectively.
 */
class GameControlFactory
{
public:
    //! Create game control with given type
    AbstractGameControl *create(const QString &type);

    //! Returns instance of GameControlFactory
    static GameControlFactory &instance();

    //! Returns avaialbe game types
    const QStringList &gameTypes() const;

private:
    GameControlFactory();
    Q_DISABLE_COPY(GameControlFactory)

    QStringList m_gameTypes;
};

#endif // GAMECONTROLFACTORY_H
