#ifndef STANDARDSUDOKUINITIALBOARDCREATOR_H
#define STANDARDSUDOKUINITIALBOARDCREATOR_H

#include <QThread>
#include <QMutex>

#include <memory>
#include <array>

/*!
 * \brief Creates initial board for standard sudoku game type
 *
 * The creation process is asynchronous. Steps taken creating table is as follows:
 *  + Create a finished table
 *      - If can not create a finished table in 4 seconds, pick a
 *        finished table from boards.db file
 *  + Remove one cell until board rate is in rage of selected difficulty and
 *    board has single solution
 *
 * \sa StandardSudokuGameControl
 */
class StandardSudokuInitialBoardCreator : public QThread
{
    Q_OBJECT

public:
    StandardSudokuInitialBoardCreator(QObject *parent = 0);

    std::array<std::array<int, 9>, 9> & result();

    int progress();
    QString progressText();

    int difficulty();
    bool setDifficulty(int value);

signals:
    void progressChanged();
    void progressTextChanged();

private:
    /*!
     * \brief This functions runs in parallel thread and performs all
     *        operations creating initial board
     */
    void run();

    //! Makes all cells empty
    void clearBoard();

    //! Assigns initial values for board
    void assignGivens(int count);

    /*!
     * \brief Tries to create a finished board. If board can not
     *        be created in time, a random finished board picked from boards.db resource file
     */
    void createFinishedBoard();

    //! Removes cells until board difficulty is
    void removeCells();
    void setProgressText(const QString &&text);

    QMutex m_boardMutex;
    std::array<std::array<int, 9>, 9> m_board;

    QMutex m_progressMutex;
    int m_progress;

    QMutex m_difficultyMutex;
    int m_difficulty;

    QMutex m_progressTextMutex;
    QString m_progressText;
};

#endif // STANDARDSUDOKUINITIALBOARDCREATOR_H
