#include "gamecontrolproxy.h"

#include "gamecontrolfactory.h"

#include <QStandardPaths>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QDir>
#include <QGuiApplication>

// TODO: Remove qDebug
#include <QDebug>

GameControlProxy::GameControlProxy(QObject *parent) :
    AbstractGameControl(parent),
    m_sourceGameControl(nullptr),
    m_database(QSqlDatabase::addDatabase("QSQLITE")),
    m_isGamePaused(false)
{
    // Initialize model
    m_listModel.setSourceModel(&m_sourceModel);

    // Initialize sqlite database
    const static QString databaseFilePath = QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).arg("sqlite.db");
    QDir databaseFileDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!databaseFileDir.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)))
        qDebug() << "Failed to mkpath of sqlite db: " << databaseFileDir.absolutePath();

    m_database.setDatabaseName(databaseFilePath);
    if (m_database.open()) {
        qDebug() << "Opened db file:" << databaseFilePath;

        // Check Scoreboard table
        QSqlQuery checkScoreboardTableQuery("SELECT name FROM sqlite_master WHERE type='table' AND name='scoreboard'");
        if (!checkScoreboardTableQuery.next()) {
            // Create scores table
            QSqlQuery createScoreBoardTableQuery("CREATE TABLE scoreboard ("
                                                 "game_type TEXT, "
                                                 "difficulty INTEGER, "
                                                 "player_name TEXT, "
                                                 "score INTEGER"
                                                 ")");
            if (!createScoreBoardTableQuery.isActive()) {
                qDebug() << "Failed to create scoreboard table; Error:"
                         << createScoreBoardTableQuery.lastError().text();
            } else {
                qDebug() << "Created scoreboard table";
            }
        }
    } else {
        qDebug() << "Fatal: Unable to open sqlite db file; File:"
                 << databaseFilePath
                 << "Error:"
                 << m_database.lastError().text();
    }

    m_scoreBoardModel.reset(new ScoreBoardModel);

    connect(static_cast<QGuiApplication*>(QGuiApplication::instance()), SIGNAL(applicationStateChanged(Qt::ApplicationState)),
            this, SLOT(handleApplicationStateChange()));
}

GameControlProxy::~GameControlProxy()
{
}

GameControlProxy &GameControlProxy::instance()
{
    static GameControlProxy instance;
    return instance;
}

SudokuBoardListModelProxy *GameControlProxy::model()
{
    return &m_listModel;
}

const QStringList &GameControlProxy::gameTypes() const
{
    return GameControlFactory::instance().gameTypes();
}

QAbstractTableModel *GameControlProxy::scoreBoardModel()
{
    return m_scoreBoardModel.data();
}

bool GameControlProxy::setGameType(const QString &type)
{
    m_gameType = type;

    setSourceGameControl(GameControlFactory::instance().create(type));

    return !m_sourceGameControl.isNull();
}

int GameControlProxy::initialTableCreationProgress()
{
    if (!m_sourceGameControl.isNull())
        return m_sourceGameControl->initialTableCreationProgress();

    return 0;
}

QString GameControlProxy::initialTableCreationProgressText()
{
    if (!m_sourceGameControl.isNull())
        return m_sourceGameControl->initialTableCreationProgressText();

    return QString();
}

void GameControlProxy::setSourceGameControl(AbstractGameControl *sourceGameControl)
{
    // This control is for first initialization of GameControlProxy
    if (!m_sourceGameControl.isNull()) {
        disconnect(m_sourceGameControl.data(), SIGNAL(gameFinished()),
                   this, SIGNAL(gameFinished()));
        disconnect(m_sourceGameControl.data(), SIGNAL(gameFinished()),
                   this, SLOT(handleGameFinished()));
        disconnect(m_sourceGameControl.data(), SIGNAL(initialTableCreationProgressChanged()),
                   this, SIGNAL(initialTableCreationProgressChanged()));
        disconnect(m_sourceGameControl.data(), SIGNAL(initialTableCreationProgressTextChanged()),
                   this, SIGNAL(initialTableCreationProgressTextChanged()));
        disconnect(m_sourceGameControl.data(), SIGNAL(creatingInitialTableFinished()),
                   this, SIGNAL(creatingInitialTableFinished()));
    }

    m_sourceGameControl.reset(sourceGameControl);
    m_sourceModel.clear();
    m_listModel.clear();

    if (!m_sourceGameControl.isNull()) {
        m_sourceGameControl->setModel(&m_sourceModel);

        connect(m_sourceGameControl.data(), SIGNAL(gameFinished()),
                this, SIGNAL(gameFinished()));
        connect(m_sourceGameControl.data(), SIGNAL(gameFinished()),
                this, SLOT(handleGameFinished()));
        connect(m_sourceGameControl.data(), SIGNAL(initialTableCreationProgressChanged()),
                this, SIGNAL(initialTableCreationProgressChanged()));
        connect(m_sourceGameControl.data(), SIGNAL(initialTableCreationProgressTextChanged()),
                this, SIGNAL(initialTableCreationProgressTextChanged()));
        connect(m_sourceGameControl.data(), SIGNAL(creatingInitialTableFinished()),
                this, SIGNAL(creatingInitialTableFinished()));
    }

    emit sourceControlChanged();
}

void GameControlProxy::createInitialTable()
{
    if (!m_sourceGameControl.isNull())
        m_sourceGameControl->createInitialTable();
}

QObject *GameControlProxy::sourceControl() const
{
    return m_sourceGameControl.data();
}

void GameControlProxy::start()
{
    if (!m_sourceGameControl.isNull())
        m_sourceGameControl->start();
}

int GameControlProxy::elapsedTime() const
{
    if (m_sourceGameControl.isNull())
        return -1;

    return m_sourceGameControl->elapsedTime();
}

int GameControlProxy::finishTime() const
{
    if (m_sourceGameControl.isNull())
        return -1;

    return m_sourceGameControl->finishTime();
}

int GameControlProxy::difficulty() const
{
    if (m_sourceGameControl.isNull())
        return -1;

    return m_sourceGameControl->difficulty();
}

void GameControlProxy::setDifficulty(int difficulty)
{
    if (!m_sourceGameControl.isNull())
        m_sourceGameControl->setDifficulty(difficulty);
}

void GameControlProxy::clearScoreBoardData(QString gameType)
{
    QSqlQuery clearScoreBoardDataQuery("DELETE FROM scoreboard "
                                       "WHERE game_type=?");
    clearScoreBoardDataQuery.addBindValue(gameType);
    if (!clearScoreBoardDataQuery.exec()) {
        qDebug() << "Failed to insert score:"
                 << clearScoreBoardDataQuery.lastError().text();
    } else {
        qDebug() << "Cleared scoreboard table data";
    }

    m_scoreBoardModel->select();
}

void GameControlProxy::resumeGameTime()
{
    m_isGamePaused = false;
    if (!m_sourceGameControl.isNull())
        m_sourceGameControl->resumeGameTime();
}

void GameControlProxy::pauseGameTime()
{
    m_isGamePaused = true;
    if (!m_sourceGameControl.isNull())
        m_sourceGameControl->pauseGameTime();
}

void GameControlProxy::handleGameFinished()
{
    QSqlQuery insertScoreQuery("INSERT INTO scoreboard "
                               "(game_type, difficulty, player_name, score) "
                               "VALUES (?, ?, ?, ?)");
    insertScoreQuery.addBindValue(m_gameType);
    insertScoreQuery.addBindValue(difficulty());
    insertScoreQuery.addBindValue("");
    insertScoreQuery.addBindValue(finishTime());
    if (!insertScoreQuery.exec()) {
        qDebug() << "Failed to insert score:"
                 << insertScoreQuery.lastError().text();
    } else {
        qDebug() << "Inserted score to db";
    }

    m_sourceModel.makeAllCellsUneditable();
    m_listModel.clear();
}

void GameControlProxy::handleApplicationStateChange()
{
    if (static_cast<QGuiApplication*>(QGuiApplication::instance())->applicationState() == Qt::ApplicationActive)
        resumeGameTime();
    else if (!m_isGamePaused)
        pauseGameTime();
}
