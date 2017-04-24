#ifndef ABSTRACTGAMECONTROL_H
#define ABSTRACTGAMECONTROL_H

#include <QObject>

#include "sudokuboardmodel.h"

class SudokuBoardModel;

/*!
 * \brief Interface for game control
 *
 * This class is part of game control implementation in proxy pattern
 * \sa StandardSudokuGameControl
 * \sa GameControlProxy
 */
class AbstractGameControl : public QObject
{
    Q_OBJECT
public:
    //! Does nothing except passing parameter parent to QObject constructor
    AbstractGameControl(QObject *parent = 0);

    //! Virtual destructor must be defined since this is a abstract class
    virtual ~AbstractGameControl();

    //! Should set sudoku board model
    virtual void setModel(SudokuBoardModel *model);

    //! Should start creating initial table model. Creation progress must be asynchronous
    virtual void createInitialTable() = 0;

    //! Should return initial table creation progress value in rage [0, 100]
    virtual int initialTableCreationProgress() = 0;
    //! Should return initial table creation progress text
    virtual QString initialTableCreationProgressText() = 0;

    //! Should clear internal values and timers
    virtual void start() = 0;

    //! Returns if game started
    virtual bool isStarted() const = 0;

    //! Should return elapsed time since start function is called
    virtual int elapsedTime() const = 0;
    //! Should set elapsed time since start function is called. Used when loading game
    virtual void setElapsedTime(int duration);
    //! Should return finish time. If game is not finished 0 must be returned
    virtual int finishTime() const = 0;

    //! Should resume paused game time
    virtual void resumeGameTime() = 0;
    //! Should pause game time
    virtual void pauseGameTime() = 0;

    //! Should return difficulty
    virtual int difficulty() const = 0;
    //! Should set difficulty. Value must be in range [1,3]
    virtual void setDifficulty(int difficulty) = 0;

    //! When user quits game, this function must be called
    virtual void close();

signals:
    //! Should be emitted when game finishes
    void gameFinished();
    //! Should be emitted when initial table creation progress changes
    void initialTableCreationProgressChanged();
    //! Should be emitted when initial table creation progress text changes
    void initialTableCreationProgressTextChanged();
    //! Should be emitted when initial table creation finished
    void creatingInitialTableFinished();
};

#endif // ABSTRACTGAMECONTROL_H
