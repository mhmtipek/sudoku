#ifndef STANDARDSUDOKUGAMECONTROL_H
#define STANDARDSUDOKUGAMECONTROL_H

#include "abstractgamecontrol.h"

#include "sudokuboardmodel.h"
#include "standardsudokuinitialboardcreator.h"

#include <QTime>

/*!
 * \brief Implementation of standard sudoku game.
 *
 * This class is part of game control implementation in proxy pattern.
 *
 * \sa AbstractGameControl
 * \sa StandardSudokuGameControl
 */
class StandardSudokuGameControl : public AbstractGameControl
{
    Q_OBJECT

public:
    StandardSudokuGameControl(QObject *parent = 0);
    virtual ~StandardSudokuGameControl();

    virtual void setModel(SudokuBoardModel *model);

    int initialTableCreationProgress();
    QString initialTableCreationProgressText();

    int difficulty() const;
    void setDifficulty(int value);

    void start();

    int elapsedTime() const;
    void setElapsedTime(int duration);
    int finishTime() const;

    void resumeGameTime();
    void pauseGameTime();

    bool isStarted() const;

private slots:
    void handleDataChanged(const QModelIndex &topLeft,
                           const QModelIndex &bottomRight,
                           const QVector<int> &roles);
    void handleBoardCreatorFinished();

private:
    void createInitialTable();

    SudokuBoardModel *m_model;

    StandardSudokuInitialBoardCreator m_boardCreator;

    int m_difficulty;

    QTime m_elapsedTime;
    int m_finishTime;
    int m_previousElapsedTimes;
};

#endif // STANDARDSUDOKUGAMECONTROL_H
