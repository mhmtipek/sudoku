#include "gamecontrolproxy.h"

#include "gamecontrolfactory.h"

#include <QStandardPaths>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QDir>
#include <QGuiApplication>
#include <QDateTime>

// TODO: Remove qDebug
#include <QDebug>

GameControlProxy::GameControlProxy(QObject *parent) :
    AbstractGameControl(parent),
    m_sourceGameControl(nullptr),
    m_database(QSqlDatabase::addDatabase("QSQLITE")),
    m_isGamePaused(false),
    m_rank(-1)
{
    // Initialize model
    m_listModel.setSourceModel(&m_sourceModel);
    connect(&m_listModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(saveBoardChange(QModelIndex,QModelIndex,QVector<int>)));

    // Initialize sqlite database
    const static QString databaseFilePath = QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).arg("sqlite.db");
    QDir databaseFileDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!databaseFileDir.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)))
        qDebug() << "Failed to mkpath of sqlite db: " << databaseFileDir.absolutePath();

    // Initialize database
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

        // Check Board Slots table
        QSqlQuery checkBoardSlotsTableQuery("SELECT name FROM sqlite_master WHERE type='table' AND name='board_slots'");
        if (!checkBoardSlotsTableQuery.next()) {
            // Create scores table
            QSqlQuery createBoardSlotsTableQuery("CREATE TABLE board_slots ("
                                                 "id INTEGER, "
                                                 "game_type TEXT, "
                                                 "board_data TEXT, "
                                                 "elapsed_time INTEGER, "
                                                 "date_time TEXT, "
                                                 "difficulty INTEGER, "
                                                 "occupancy_percentage INTEGER"
                                                 ")");
            if (!createBoardSlotsTableQuery.isActive()) {
                qDebug() << "Failed to create board_slots table; Error:"
                         << createBoardSlotsTableQuery.lastError().text();
            } else {
                qDebug() << "Created board_slots table";

                QSqlQuery insertSlotsQuery;
                for (int x = 0; x < 5; ++x) {
                    if (!insertSlotsQuery.exec(QString("INSERT INTO board_slots VALUES ('%1', '', '', 0, '', 0, 0)").arg(x))) {
                        qDebug() << "Inserting slot into database failed:"
                                 << insertSlotsQuery.lastError().text();
                    }
                }
            }
        }

        // Check live game data table
        QSqlQuery checkLiveGameDataTableQuery("SELECT name FROM sqlite_master WHERE type='table' AND name='live_game_data'");
        if (!checkLiveGameDataTableQuery.next()) {
            // Create live game data table
            QSqlQuery createLiveGameDataTableQuery("CREATE TABLE live_game_data ("
                                                   "game_active INTEGER, "
                                                   "game_type TEXT, "
                                                   "board_data TEXT, "
                                                   "elapsed_time INTEGER, "
                                                   "difficulty INTEGER"
                                                   ")");
            if (!createLiveGameDataTableQuery.isActive()) {
                qDebug() << "Failed to create live_game_data table; Error:"
                         << createLiveGameDataTableQuery.lastError().text();
            } else {
                qDebug() << "Created live_game_data table";

                QSqlQuery insertDataQuery(QString("INSERT INTO live_game_data VALUES (0, '', '', 0, 0)"));
                if (!insertDataQuery.isActive()) {
                    qDebug() << "Inserting live game data row into database failed:"
                             << insertDataQuery.lastError().text();
                }
            }
        }
    } else {
        qDebug() << "Fatal: Unable to open sqlite db file; File:"
                 << databaseFilePath
                 << "Error:"
                 << m_database.lastError().text();
    }

    m_scoreBoardModel.reset(new ScoreBoardModel);
    m_boardSlotsModel.reset(new BoardSlotsModel);

    connect(static_cast<QGuiApplication*>(QGuiApplication::instance()), SIGNAL(applicationStateChanged(Qt::ApplicationState)),
            this, SLOT(handleApplicationStateChange()));

    m_updateLiveGameTimeTimer.setInterval(1000);
    connect(&m_updateLiveGameTimeTimer, SIGNAL(timeout()),
            this, SLOT(updateLiveGameTime()));
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

QAbstractTableModel *GameControlProxy::boardSlotsModel()
{
    return m_boardSlotsModel.data();
}

bool GameControlProxy::setGameType(const QString &type)
{
    m_gameType = type;

    setSourceGameControl(GameControlFactory::instance().create(type));
    updateGameTypeInLiveGameData();

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
                   this, SLOT(handleGameFinished()));
        disconnect(m_sourceGameControl.data(), SIGNAL(initialTableCreationProgressChanged()),
                   this, SIGNAL(initialTableCreationProgressChanged()));
        disconnect(m_sourceGameControl.data(), SIGNAL(initialTableCreationProgressTextChanged()),
                   this, SIGNAL(initialTableCreationProgressTextChanged()));
        disconnect(m_sourceGameControl.data(), SIGNAL(creatingInitialTableFinished()),
                   this, SIGNAL(creatingInitialTableFinished()));
        disconnect(m_sourceGameControl.data(), SIGNAL(creatingInitialTableFinished()),
                   this, SLOT(enableLiveGameData()));
    }

    m_sourceGameControl.reset(sourceGameControl);
    m_sourceModel.clear();
    m_listModel.clear();
    m_rank = -1;
    clearLiveGameData();

    if (!m_sourceGameControl.isNull()) {
        m_sourceGameControl->setModel(&m_sourceModel);

        connect(m_sourceGameControl.data(), SIGNAL(gameFinished()),
                this, SLOT(handleGameFinished()));
        connect(m_sourceGameControl.data(), SIGNAL(initialTableCreationProgressChanged()),
                this, SIGNAL(initialTableCreationProgressChanged()));
        connect(m_sourceGameControl.data(), SIGNAL(initialTableCreationProgressTextChanged()),
                this, SIGNAL(initialTableCreationProgressTextChanged()));
        connect(m_sourceGameControl.data(), SIGNAL(creatingInitialTableFinished()),
                this, SIGNAL(creatingInitialTableFinished()));
        connect(m_sourceGameControl.data(), SIGNAL(creatingInitialTableFinished()),
                this, SLOT(enableLiveGameData()));
    }

    emit sourceControlChanged();
}

void GameControlProxy::clearLiveGameData()
{
    qDebug() << "clearLiveGameData";

    QSqlQuery clearQuery("UPDATE live_game_data set game_active=0, game_type='', board_data='', elapsed_time=0, difficulty=0");

    if (!clearQuery.isActive()) {
        qDebug() << "Failed to clear live game data;"
                 << clearQuery.lastError().text();
    }

    emit previousGameUnfinishedChanged();
}

void GameControlProxy::enableLiveGameData()
{
    setLiveGameDataEnabled(true);
}

void GameControlProxy::setLiveGameDataEnabled(bool ok)
{
    QSqlQuery updateQuery("UPDATE live_game_data set game_active=?");
    updateQuery.bindValue(0, ok ? 1 : 0);

    if (!updateQuery.exec()) {
        qDebug() << "Failed to set live game data" << (ok ? "enabled;" : "disabled;")
                 << updateQuery.lastError().text();
    }

    emit previousGameUnfinishedChanged();
}

void GameControlProxy::updateLiveGameData()
{
    saveBoardChange(QModelIndex(), QModelIndex(), QVector<int>() << SudokuBoardModel::ValueRole);
    updateLiveGameTime();
    updateGameTypeInLiveGameData();
}

void GameControlProxy::updateGameTypeInLiveGameData()
{
    QSqlQuery updateQuery("UPDATE live_game_data set game_type=?");
    updateQuery.bindValue(0, m_gameType);

    if (!updateQuery.exec()) {
        qDebug() << "Failed to set game type in live_game_data table;"
                 << updateQuery.lastError().text();
    }
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
    if (!m_sourceGameControl.isNull()) {
        m_sourceGameControl->start();
        m_updateLiveGameTimeTimer.start();
    }
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

    m_scoreBoardModel->refresh();
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

int GameControlProxy::rank() const
{
    return m_rank;
}

void GameControlProxy::saveBoardToSlot(int id)
{
    int occupancyPercentage = 0;
    for (int x = 0; x < m_listModel.rowCount(); ++x) {
        if (m_listModel.data(m_listModel.index(x, 0)).toInt() != EMPTY_CELL_VALUE)
            ++occupancyPercentage;
    }

    QString boardData;
    for (int x = 0; x < m_listModel.rowCount(); ++x)
        boardData += QString("%1;%2,").arg(m_listModel.data(m_listModel.index(x, 0)).toInt()).arg(m_listModel.data(m_listModel.createIndex(x, 0), SudokuBoardModel::EditableRole).toBool() ? "e" : "");
    boardData.chop(1);

    QSqlQuery insertQuery("UPDATE board_slots SET game_type=?, board_data=?, elapsed_time=?, date_time=?, difficulty=?, occupancy_percentage=? WHERE id=?");
    insertQuery.bindValue(0, m_gameType);
    insertQuery.bindValue(1, boardData);
    insertQuery.bindValue(2, m_sourceGameControl->elapsedTime());
    insertQuery.bindValue(3, QDateTime::currentDateTime().toString("hh:mm:ss dd.MM.yyyy"));
    insertQuery.bindValue(4, m_sourceGameControl->difficulty());
    insertQuery.bindValue(5, static_cast<int>((occupancyPercentage * 100) / 81));
    insertQuery.bindValue(6, id);

    if (!insertQuery.exec()) {
        qDebug() << "Failed to save board to slot" << id
                 << ";" << insertQuery.lastError().text();
    } else {
        m_boardSlotsModel->select();
    }
}

bool GameControlProxy::loadSavedGame(int id)
{
    if (!m_sourceGameControl.isNull()) {
        setGameType("");
    }

    QSqlQuery selectQuery("SELECT * FROM board_slots WHERE id=?");
    selectQuery.bindValue(0, id);
    if (!selectQuery.exec())
        return false;

    if (!selectQuery.next())
        return false;

    const QStringList boardValues = selectQuery.value("board_data").toString().split(",");
    if (boardValues.size() != m_listModel.rowCount())
        return false;

    setSourceGameControl(GameControlFactory::instance().create(selectQuery.value("game_type").toString()));
    if (m_sourceGameControl.isNull())
        return false;

    m_gameType = selectQuery.value("game_type").toString();

    m_sourceGameControl->setDifficulty(selectQuery.value("difficulty").toInt());
    for (int x = 0; x < m_listModel.rowCount(); ++x) {
        const QStringList cellData = boardValues.at(x).split(";");
        m_listModel.setData(m_listModel.index(x, 0), cellData.at(0).toInt(), Qt::DisplayRole);
        if (cellData[1].contains('e')) {
            const QModelIndex index = m_listModel.mapToSource(m_listModel.createIndex(x, 0));
            m_sourceModel.setCellEditable(index.row(), index.column());
        }
    }

    saveBoardChange(QModelIndex(), QModelIndex(), QVector<int>() << SudokuBoardModel::ValueRole);

    start();
    m_sourceGameControl->setElapsedTime(selectQuery.value("elapsed_time").toInt());

    updateLiveGameData();
    setLiveGameDataEnabled(true);

    return true;
}

bool GameControlProxy::isStarted() const
{
    return m_sourceGameControl.isNull() ? false
                                        : m_sourceGameControl->isStarted();
}

bool GameControlProxy::isPreviousGameUnfinished() const
{
    QSqlQuery query("SELECT game_active from live_game_data");
    if (!query.isActive()) {
        qDebug() << "Failed to check previous game active:"
                 << query.lastError().text();
        return false;
    }

    query.next();

    return query.value(0).toInt() != 0;
}

void GameControlProxy::continueLastGame()
{
    qDebug() << "Continue last game";

    QSqlQuery selectQuery("SELECT * FROM live_game_data");
    if (!selectQuery.isActive()) {
        qDebug() << "Failed to get live game data:" << selectQuery.lastError().text();
        return;
    }

    if (!selectQuery.next()) {
        qDebug() << "Error: live_game_data table is empty";
        return;
    }

    const QStringList boardValues = selectQuery.value("board_data").toString().split(",");
    if (boardValues.size() != m_listModel.rowCount()) {
        qDebug() << "Error: live_game_data table is empty";
        return;
    }

    m_gameType = selectQuery.value("game_type").toString();

    updateGameTypeInLiveGameData();
    const int difficulty = selectQuery.value("difficulty").toInt();

    setSourceGameControl(GameControlFactory::instance().create(m_gameType));
    if (m_sourceGameControl.isNull())
        return;

    m_sourceGameControl->setDifficulty(difficulty);
    for (int x = 0; x < m_listModel.rowCount(); ++x) {
        const QStringList cellData = boardValues.at(x).split(";");
        m_listModel.setData(m_listModel.index(x, 0), cellData.at(0).toInt(), Qt::DisplayRole);
        if (cellData[1].contains('e')) {
            const QModelIndex index = m_listModel.mapToSource(m_listModel.createIndex(x, 0));
            m_sourceModel.setCellEditable(index.row(), index.column());
        }
    }

    saveBoardChange(QModelIndex(), QModelIndex(), QVector<int>() << SudokuBoardModel::ValueRole);

    start();
    m_sourceGameControl->setElapsedTime(selectQuery.value("elapsed_time").toInt());

    updateLiveGameData();

    setLiveGameDataEnabled(true);
}

void GameControlProxy::close()
{
    clearLiveGameData();
}

void GameControlProxy::handleGameFinished()
{
    m_updateLiveGameTimeTimer.stop();
    setLiveGameDataEnabled(false);

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

    QSqlQuery rankQuery("SELECT * FROM scoreboard WHERE difficulty=? ORDER BY score");
    rankQuery.addBindValue(m_sourceGameControl->difficulty());
    if (!rankQuery.exec()) {
        qDebug() << "Failed to query scores:"
                 << rankQuery.lastError().text();
    } else {
        int rank = 1;
        while (rankQuery.next()) {
            if (rankQuery.value("score").toInt() == finishTime())
                break;
            ++rank;
        }

        m_rank = rank;
        emit rankChanged();
    }

    m_sourceModel.makeAllCellsUneditable();
    m_listModel.clear();

    emit gameFinished();
}

void GameControlProxy::handleApplicationStateChange()
{
    if (static_cast<QGuiApplication*>(QGuiApplication::instance())->applicationState() == Qt::ApplicationActive)
        resumeGameTime();
    else if (!m_isGamePaused)
        pauseGameTime();
}

void GameControlProxy::saveBoardChange(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)

    if (!roles.contains(SudokuBoardModel::ValueRole) && !roles.contains(SudokuBoardModel::EditableRole) && !roles.contains(Qt::DisplayRole))
        return;

    QString boardData;
    for (int x = 0; x < m_listModel.rowCount(); ++x)
        boardData += QString("%1;%2,")
                .arg(m_listModel.data(m_listModel.index(x, 0)).toInt())
                .arg(m_listModel.data(m_listModel.createIndex(x, 0), SudokuBoardModel::EditableRole).toBool() ? "e" : "");
    boardData.chop(1);

    QSqlQuery updateQuery("UPDATE live_game_data SET board_data=?");
    updateQuery.bindValue(0, boardData);
    if (!updateQuery.exec()) {
        qDebug() << "Failed to save board data to live_game_data table;"
                 << updateQuery.lastError().text();
    }
}

void GameControlProxy::updateLiveGameTime()
{
    QSqlQuery updateQuery("UPDATE live_game_data set elapsed_time=?");
    updateQuery.bindValue(0, m_sourceGameControl->elapsedTime());

    if (!updateQuery.exec()) {
        qDebug() << "Failed to update game time;"
                 << updateQuery.lastError().text();
    }
}
