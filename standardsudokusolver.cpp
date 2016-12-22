#include "standardsudokusolver.h"
#include "sudokuboardcelldata.h"
#include "standardsudokusearchtree.h"
#include "utils.h"

//StandardSudokuSolver::StandardSudokuSolver(QObject *parent)
//    : QObject(parent)
//{

//}

//StandardSudokuSolver::~StandardSudokuSolver()
//{

//}

bool StandardSudokuSolver::isMoveValid(const std::array<std::array<int, 9>, 9> &boardData, int row, int col, int val)
{
    return availableValues(boardData, row, col).contains(val);
}

QVector<int> StandardSudokuSolver::availableValues(const std::array<std::array<int, 9>, 9> &boardData, int row, int col)
{
    if (row < 0 || row > 8 || col < 0 || col > 8)
        return QVector<int>();

    QVector<int> possibilities = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    for (int x = 0; x < 9; ++x) {
        if (boardData[row][x] != EMPTY_CELL_VALUE)
            possibilities.removeOne(boardData[row][x]);
    }

    for (int x = 0; x < 9; ++x) {
        if (boardData[x][col] != EMPTY_CELL_VALUE)
            possibilities.removeOne(boardData[x][col]);
    }

    const Index quadraticIndex = mapFromLinear({row, col});
    for (int x = 0; x < 9; ++x) {
        const Index linearIndex = mapToLinear({quadraticIndex.row, x});
        if (boardData[linearIndex.row][linearIndex.column] != EMPTY_CELL_VALUE)
            possibilities.removeOne(boardData[linearIndex.row][linearIndex.column]);
    }

    return possibilities;
}

bool StandardSudokuSolver::solve(std::array<std::array<int, 9>, 9> &board)
{
    QScopedPointer<StandardSudokuSearchTree> m_searchTree(new StandardSudokuSearchTree(board));

    m_searchTree->search_MultiThread();

    return m_searchTree->isSuccessfull();
}

int StandardSudokuSolver::solutionCount(std::array<std::array<int, 9>, 9> &board)
{
    QScopedPointer<StandardSudokuSearchTree> m_searchTree(new StandardSudokuSearchTree(board, true));

    m_searchTree->search_MultiThread();

    return m_searchTree->finishedNodesCount();
}

float StandardSudokuSolver::boardRate(const std::array<std::array<int, 9>, 9> &board)
{
    static int scores[] = {1, 2, 4, 7, 10, 14, 18, 23, 30};

    float totalRate = 0;

    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (board[row][col] == EMPTY_CELL_VALUE)
                totalRate += scores[availableValues(board, row, col).size()];
        }
    }

    return totalRate;
}

