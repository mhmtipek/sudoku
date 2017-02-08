#include "standardsudokuinitialboardcreator.h"

#include <QMutexLocker>
#include <QtMath>
#include <QTime>
#include <QVector>
#include <QFile>

#include "sudokuboardcelldata.h"
#include "standardsudokusolver.h"

#include <QDebug>

static const QVector<int> difficultyRates({200, 360, 500, 640});

//! Returns difficulty of given rate
static int difficultyByBoardRate(float rate)
{
    if (rate < difficultyRates[0])
        return 0;

    if (rate < difficultyRates[1])
        return 1;

    if (rate < difficultyRates[2])
        return 2;

    if (rate < difficultyRates[3])
        return 3;

    return 4;
}

inline int waitTimeForDifficulty(int difficulty)
{
    if (difficulty == 4)
        return 8000;

    return 4000;
}

StandardSudokuInitialBoardCreator::StandardSudokuInitialBoardCreator(QObject *parent) :
    QThread(parent),
    m_progress(0),
    m_difficulty(1)
{

}

std::array<std::array<int, 9>, 9> &StandardSudokuInitialBoardCreator::result()
{
    QMutexLocker locker(&m_boardMutex);

    return m_board;
}

int StandardSudokuInitialBoardCreator::progress()
{
    QMutexLocker locker(&m_progressMutex);

    return m_progress;
}

QString StandardSudokuInitialBoardCreator::progressText()
{
    QMutexLocker locker(&m_progressTextMutex);

    return m_progressText;
}

int StandardSudokuInitialBoardCreator::difficulty()
{
    QMutexLocker locker(&m_difficultyMutex);

    return m_difficulty;
}

bool StandardSudokuInitialBoardCreator::setDifficulty(int value)
{
    if (isRunning())
        return false;

    if (value < 1 || value > 5)
        return false;

    m_difficulty = value;

    return true;
}

void StandardSudokuInitialBoardCreator::run()
{
    QMutexLocker locker(&m_boardMutex);

    setProgressText(tr("Creating board"));
    createFinishedBoard();

    setProgressText(tr("Adjusting difficulty"));
    removeCells();

    // Print board created
    for (int row = 0; row < 9; ++row) {
        QString rowString;
        for (int col = 0; col < 9; ++col)
            rowString += QString("%1 ").arg(m_board[row][col]);

        qDebug() << rowString;
    }
}

void StandardSudokuInitialBoardCreator::clearBoard()
{
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col)
            m_board[row][col] = EMPTY_CELL_VALUE;
    }
}

/*!
 * \brief Creates a finished board
 *
 * This function is optimized for creating a finished board as soon as possible.
 * First, random values are assign at random cells. Then StandardSudokuSolver::solve
 * function is called to get a result. If a solution is not found, try to find
 * again for 4 seconds.
 *
 * Number of initial values are determined by testing each initial value count
 * and measuring average solution time.
 *
 */
void StandardSudokuInitialBoardCreator::createFinishedBoard()
{
    srand(time(0));

    QTime elapsedTime;
    elapsedTime.start();
    do {
        clearBoard();
        assignGivens(32);

        if (StandardSudokuSolver::solve(m_board))
            break;

    } while(elapsedTime.elapsed() < 4000);

    if (!StandardSudokuSolver::isBoardFinished(m_board)) {
        qDebug() << "could not find a solution in 4 seconds. getting from db";

        QFile boards(QStringLiteral(":/boards.db"));
        boards.open(QIODevice::ReadOnly);

        const int index = rand() % 10001;
        qDebug() << "getting board at index" << index;

        boards.seek(index * 81 * sizeof(int));
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col)
                boards.read(reinterpret_cast<char *>(&(m_board.at(row).at(col))), sizeof(int));
        }

        boards.close();
    } else {
        qDebug() << "found solution in" << elapsedTime.elapsed() << "ms";
    }
}

/*!
 * \brief This functions removes cells until board rate reaches desired
 *        difficulty and has single solution.
 */
void StandardSudokuInitialBoardCreator::removeCells()
{
    qDebug() << "Adjusting difficulty";

    QList<int> nonEmptyIndexes;
    for (int x = 0; x < 81; ++x)
        nonEmptyIndexes.append(x);

    float rate = 0;
    std::array<std::array<int, 9>, 9> testBoard;
    QTime elapsedTime;

    // Remove cell until board rate reaches desired difficulty
    while (difficultyByBoardRate(rate) < m_difficulty) {
        const int index = rand() % nonEmptyIndexes.size();

        // Remove random cell
        testBoard = m_board;
        testBoard[qFloor(nonEmptyIndexes[index] / 9)][nonEmptyIndexes[index] % 9] = EMPTY_CELL_VALUE;

        qDebug() << "Finding solution count with" << nonEmptyIndexes.size() << "givens";
        if (StandardSudokuSolver::solutionCount(testBoard) > 1) {
            qDebug() << "Reverted:" << rate;

            if (elapsedTime.elapsed() >= waitTimeForDifficulty(m_difficulty)) {
                qDebug() << "All I can do is" << rate;
                break;
            }
        } else {
            m_board[qFloor(nonEmptyIndexes[index] / 9)][nonEmptyIndexes[index] % 9] = EMPTY_CELL_VALUE;

            rate = StandardSudokuSolver::boardRate(m_board);
            qDebug() << "Proceeded:" << rate;

            nonEmptyIndexes.removeAt(index);
            elapsedTime.start();
        }
    }

    qDebug() << "Board rate:" << StandardSudokuSolver::boardRate(m_board);
    qDebug() << "Non-empty index count:" << nonEmptyIndexes.size();
    testBoard = m_board;
    qDebug() << "Solution count:" << StandardSudokuSolver::solutionCount(testBoard);
}

void StandardSudokuInitialBoardCreator::setProgressText(const QString &&text)
{
    m_progressTextMutex.lock();
    m_progressText = text;
    m_progressTextMutex.unlock();

    emit progressTextChanged();
}

/*!
 * \brief Assigns random values at random cells given count times
 * \param count Number of assignments to do
 */
void StandardSudokuInitialBoardCreator::assignGivens(int count)
{
    QVector<int> indexes;

    QVector<int> allIndexes(81);
    for (int x = 0; x < allIndexes.size(); ++x)
        allIndexes[x] = x;

    for (int x = 0; x < count; ++x) {
        int value = rand() % allIndexes.size();
        indexes.append(allIndexes[value]);
        allIndexes.removeAt(value);
    }

    int assignCount = 0;
    for (int x = 0; x < indexes.size(); ++x) {
        const QVector<int> & availableValues = StandardSudokuSolver::availableValues(m_board, indexes[x] / 9, indexes[x] % 9);
        if (availableValues.size() != 0) {
            int value = availableValues[rand() % availableValues.size()];
            m_board[qFloor(indexes[x] / 9)][indexes[x] % 9] = value;
            ++assignCount;
        }
    }
}

