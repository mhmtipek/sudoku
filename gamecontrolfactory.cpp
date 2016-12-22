#include "gamecontrolfactory.h"

#include "standardsudokugamecontrol.h"

AbstractGameControl *GameControlFactory::create(const QString &type)
{
    if (type == "standard")
        return new StandardSudokuGameControl;

    return nullptr;
}

GameControlFactory &GameControlFactory::instance()
{
    static GameControlFactory m_instance;

    return m_instance;
}

const QStringList &GameControlFactory::gameTypes() const
{
    return m_gameTypes;
}

GameControlFactory::GameControlFactory()
{
    m_gameTypes << "standard";
}
