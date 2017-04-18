#ifndef STANDARDSUDOKUSOLVER_H
#define STANDARDSUDOKUSOLVER_H

#include <QVector>
#include <array>

/*!
 * \brief Provides a unified interface for functions related solving sudoku board
 */
class StandardSudokuSolver
{
public:
    //! No need for constructor
    StandardSudokuSolver() = delete;
    //! No need for destructor
    ~StandardSudokuSolver() = delete;

    /*!
     * \brief Solves the board. If there is more than one solution, first solution is
     * \param board Board to operate on
     * \return If board has a solution, returns true else false
     */
    static bool solve(std::array<std::array<int, 9>, 9> &board);

    /*!
     * \brief Finds the solution count of board
     * \param board Board to operate
     * \return Number of solutions
     */
    static int solutionCount(std::array<std::array<int, 9>, 9> &board);

    /*!
     * \brief Checks for a conflict if value is put in cell that pointed by row, col
     * \param boardData Board data
     * \param row Row of cell
     * \param col Column of cell
     * \param val Value to check
     * \return True if valid, false if invalid
     */
    static bool isMoveValid(const std::array<std::array<int, 9>, 9> &boardData,
                            int row, int col, int val);

    /*!
     * \brief Returns available values that can be put in cell
     * \param boardData Board to find available values
     * \param row Row of cell
     * \param col Column of cell
     * \return List of available values
     */
    static QVector<int> availableValues(const std::array<std::array<int, 9>, 9> &boardData,
                                        int row, int col);

    /*!
     * \brief Checks board if it is finished
     *
     *  Made this function template because it is used by both std::array<std::array<int, 9>, 9>
     *  and std::array<std::array<SudokuBoardCellData, 9>, 9>
     *
     * \param board Board data
     * \return True if board is finished, false if board is not finished
     */
    template <typename T>
    static bool isBoardFinished(const std::array<std::array<T, 9>, 9> &board) {
        for (int row = 0; row < 9; ++row) {
            for (int value = 1; value <= 9; ++value) {
                if (std::find(board[row].begin(), board[row].end(), value) == board[row].end())
                    return false;
            }
        }

        return true;
    }

    /*!
     * \brief Calculates rate of the board
     *
     * Calculation is performed as follow:
     *
     * Empty cells determines the rate of the board and empty cells are rated
     * according to how many values it can get. For example;
     *
     * Cell 1, 3 is empty. Available values that can be inserted in this cell
     * obtained from availableValues function. If available value array has length of;
     *  - 1: cell rate is 1
     *  - 2: cell rate is 2
     *  - 3: cell rate is 4
     *  ...
     *  ..
     *  (All scores are defined in function)
     *
     * The sum of all empty cell rates is the rate of the board.
     *
     * \param board Board data
     *
     * \return Rate of the board
     *
     */
    static float boardRate(const std::array<std::array<int, 9>, 9> &board);
};

#endif // STANDARDSUDOKUSOLVER_H
