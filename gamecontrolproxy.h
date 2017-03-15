#ifndef GAMECONTROLPROXY_H
#define GAMECONTROLPROXY_H

#include "abstractgamecontrol.h"

#include "sudokuboardlistmodelproxy.h"
#include "sudokuboardmodel.h"
#include "scoreboardmodel.h"

#include <QScopedPointer>
#include <QSqlDatabase>

/*!
 * \brief Proxy class for game control. This class provides interface for QML
 *
 * This class is part of game control implementation in proxy pattern.
 *
 * \sa AbstractGameControl
 * \sa StandardSudokuGameControl
 */
class GameControlProxy : public AbstractGameControl
{
    Q_OBJECT

    //! This property will be used in SudokuBoard.qml file as model of sudoku board
    Q_PROPERTY(SudokuBoardListModelProxy* model READ model CONSTANT)

    //! Available game types
    Q_PROPERTY(const QStringList gameTypes READ gameTypes CONSTANT)

    //! Initial table creation progress in rage [0, 100]
    Q_PROPERTY(int initialTableCreationProgress READ initialTableCreationProgress NOTIFY initialTableCreationProgressChanged)

    //! Initial table creation progress text
    Q_PROPERTY(QString initialTableCreationProgressText READ initialTableCreationProgressText NOTIFY initialTableCreationProgressTextChanged)

    //! Returns source game control object. \sa StandardSudokuGameControl
    Q_PROPERTY(QObject* sourceControl READ sourceControl NOTIFY sourceControlChanged)

    //! Scoreboard model that used in ScoreBoardPage.qml
    Q_PROPERTY(QAbstractTableModel* scoreBoardModel READ scoreBoardModel CONSTANT)

    //! Rank of finish time
    Q_PROPERTY(int rank READ rank NOTIFY rankChanged)

public:
    //! returns singleton instance of GameControlProxy
    static GameControlProxy& instance();

    //! returns model used for sudoku board
    SudokuBoardListModelProxy* model();

    //! returns available game types
    const QStringList &gameTypes() const;

    //! returns score board model
    QAbstractTableModel* scoreBoardModel();

    /*!
     * \brief Sets game type. Parameter should be one of the values returned by gameTypes function
     * \sa gameType
     * \param board Board to operate
     * \return Number of solutions
     */
    Q_INVOKABLE bool setGameType(const QString &type);

    //! \see initialTableCreationProgress property
    int initialTableCreationProgress();
    //! \see initialTableCreationProgressText property
    QString initialTableCreationProgressText();

    /*!
     * \brief Creates initial table that user should start to play.
     *        Difficulty should be set before calling this function
     * \sa gameType
     * \param board Board to operate
     * \return Number of solutions
     */
    Q_INVOKABLE void createInitialTable();

    //! Returns source game control object
    QObject* sourceControl() const;

    //! \see AbstractGameControl::start
    Q_INVOKABLE void start();
    //! \see AbstractGameControl::elapsedTime
    Q_INVOKABLE int elapsedTime() const;
    //! \see AbstractGameControl::finishTime
    Q_INVOKABLE int finishTime() const;

    //! \see AbstractGameControl::difficulty
    Q_INVOKABLE int difficulty() const;
    //! \see AbstractGameControl::setDifficulty
    Q_INVOKABLE void setDifficulty(int difficulty);

    /*!
     * \brief Clears all score data of gameType has
     * \param gameType Type of game to clear scores
     */
    Q_INVOKABLE void clearScoreBoardData(QString gameType);

    //! \see AbstractGameControl::resumeGameTime
    Q_INVOKABLE void resumeGameTime();
    //! \see AbstractGameControl::pauseGameTime
    Q_INVOKABLE void pauseGameTime();

    int rank() const;

signals:
    //! Emitted when source game control is changed
    void sourceControlChanged();

    /*!
     * \brief This signal is already defined in AbstractGameControl
     *        but it had to be defined again because Q_PROPERTY gives
     *        error on compile time if it is not defined in
     * \see AbstractGameControl::initialTableCreationProgressTextChanged
     */

    void initialTableCreationProgressTextChanged();
    /*!
     * \brief This signal is already defined in AbstractGameControl
     *        but it had to be defined again because Q_PROPERTY gives
     *        error on compile time if it is not defined in
     * \see AbstractGameControl::initialTableCreationProgressChanged
     */
    void initialTableCreationProgressChanged();

    /*!
     * \brief Is called when user finishes game.
     *
     */
    void rankChanged();

private slots:
    /*!
     * \brief Handles gameFinished signal emitted from source game control
     *
     *  - Inserts score to scoreboard table
     *  - Makes all cells uneditable
     *  - Clears undo stack
     */
    void handleGameFinished();

    /*!
     * \brief Is called when application goes to background or foreground.
     *        When app is in background, game time should be paused.
     */
    void handleApplicationStateChange();

private:
    /*!
     * \brief Initializes proxy sudoku board model, SQLITE database and scoreboard
     *        model. Defined in private because class is implemented singleton
     *
     */
    GameControlProxy(QObject *parent = 0);
    //! Disable copying
    Q_DISABLE_COPY(GameControlProxy)
    virtual ~GameControlProxy();

    void setSourceGameControl(AbstractGameControl *sourceGameControl);

    SudokuBoardModel m_sourceModel;
    SudokuBoardListModelProxy m_listModel;

    QScopedPointer<AbstractGameControl> m_sourceGameControl;
    QString m_gameType;

    QSqlDatabase m_database;
    QScopedPointer<ScoreBoardModel> m_scoreBoardModel;

    bool m_isGamePaused;

    int m_rank;
};

#endif // GAMECONTROLPROXY_H
