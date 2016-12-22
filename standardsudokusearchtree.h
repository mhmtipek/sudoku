#ifndef STANDARDSUDOKUSEARCHTREE_H
#define STANDARDSUDOKUSEARCHTREE_H

#include <array>
#include <QtGlobal>
#include <QScopedPointer>

//! Forward decleration for PIMPL idiom
class StandardSudokuSearchTreePrivate;

/*!
 * \brief Performs searching operation in given board to find a solution.
 *
 * Constructs a tree from board data and performs Depth First Search on tree.
 * Searching can be performed on single thread or multiple threads.
 */
class StandardSudokuSearchTree
{
public:
    /*!
     * \brief Takes all values needed
     *
     * \param board Board to search
     * \param findAllSolutions If false, searching stops if a solution is
     *        found. If true, searches for all possible solutions
     */
    explicit StandardSudokuSearchTree(std::array<std::array<int, 9>, 9> &board, bool findAllSolutions = false);
    ~StandardSudokuSearchTree();

    //! Searches tree on multiple threads
    void search_MultiThread();

    //! Searches tree on single thread. Single thread search is not used atm
    void search_singleThread();

    //! Returns true if a solution is found, else returns false
    bool isSuccessfull();

    //! Returns number of solution found
    int finishedNodesCount() const;

private:
    Q_DISABLE_COPY(StandardSudokuSearchTree)

    //! Used PIMPL to hide internal structures and have a clear interface
    const QScopedPointer<StandardSudokuSearchTreePrivate> d_ptr;
    Q_DECLARE_PRIVATE(StandardSudokuSearchTree)

};

#endif // STANDARDSUDOKUSEARCHTREE_H
