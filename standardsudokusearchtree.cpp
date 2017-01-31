#include "standardsudokusearchtree.h"

#include "sudokuboardcelldata.h"
#include "utils.h"
#include "standardsudokusolver.h"

#include <QSharedPointer>
#include <QWeakPointer>
#include <QLinkedList>
#include <QStack>
#include <QThread>
#include <QtConcurrent>
#include <QAtomicInt>

#define INVALID_INDEX -1

// Helper structures
/*!
 * \brief Tree node for single thread searching
 */
struct Node {
    Node() : index({INVALID_INDEX, INVALID_INDEX}), value(EMPTY_CELL_VALUE) {}

    //! Index of node
    Index index;
    //! Value of node
    int value;
    //! Sub-nodes.
    QLinkedList<QSharedPointer<Node> > subNodes;
    //! Parent node. Used when filling board until head node.
    QWeakPointer<Node> parentNode;
};

/*!
 * \brief Tree node for multithread searching
 */
struct MultiThreadSearchNode {
    //! Node data
    QSharedPointer<Node> node;

    //! Board to search. Each node has copy of board to prevent
    //! race condition working on single board
    std::array<std::array<int, 9>, 9> board;

    //! If a thread finds a solution, marks this int. If cancelWhenFound
    //! is set to true, thread stops running
    QAtomicInt *found;

    //! If set to true, cancels searching when another thread finds a
    //! solution. If set to false, continues to search
    bool cancelWhenFound;
};

/*!
 * \brief Fills given board until head node by going upper node
 * \param board Board to fill
 * \param startNode Node to start filling
 */
void fillBoardTillHead(std::array<std::array<int, 9>, 9> &board,
                       QSharedPointer<Node> startNode)
{
    QSharedPointer<Node> current(startNode);

    while (!current.isNull()) {
        board[current->index.row][current->index.column] = current->value;
        current = current->parentNode;
    }
}

/*!
 * \brief Performs Depth First Search (DFS) on given node
 * \param node Node to search
 * \return List of nodes that has a solution
 */
QLinkedList<QSharedPointer<Node> > searchNode(MultiThreadSearchNode node)
{
    QLinkedList<QSharedPointer<Node> > result;

    // Use stack instead of recursive functions to prevent stack overflow
    QStack<QSharedPointer<Node> > nodesList;
    nodesList.push(node.node);

    std::array<std::array<int, 9>, 9> testBoard;
    while (!nodesList.empty()) {
        if (node.cancelWhenFound && *(node.found) != 0)
            return result;

        QSharedPointer<Node> currentNode = nodesList.pop();

        int childNodeColumn;
        int childNodeRow;

        if (currentNode->index.column == 8) {
            if (currentNode->index.row == 8) {
                result += currentNode;
                node.found->store(1);
                if (node.cancelWhenFound)
                    return result;
                continue;
            } else {
                childNodeColumn = 0;
                childNodeRow = currentNode->index.row + 1;
            }
        } else {
            childNodeColumn = currentNode->index.column + 1;
            childNodeRow = currentNode->index.row;
        }

        if (node.board[childNodeRow][childNodeColumn] == EMPTY_CELL_VALUE) {
            testBoard = node.board;
            fillBoardTillHead(testBoard, currentNode);
            const QVector<int> availableValues = StandardSudokuSolver::availableValues(testBoard,
                                                                                       childNodeRow,
                                                                                       childNodeColumn);
            for (int x = 0; x < availableValues.size(); ++x) {
                QSharedPointer<Node> child(new Node);
                child->parentNode = currentNode;
                child->index.column = childNodeColumn;
                child->index.row = childNodeRow;
                child->value = availableValues.at(x);
                currentNode->subNodes.append(child);
            }
        } else {
            QSharedPointer<Node> child(new Node);
            child->parentNode = currentNode;
            child->index.column = childNodeColumn;
            child->index.row = childNodeRow;
            child->value = node.board[childNodeRow][childNodeColumn];
            currentNode->subNodes.append(child);
        }

        for (auto it = currentNode->subNodes.begin(); it != currentNode->subNodes.end(); ++it)
            nodesList.push(*it);
    }

    return result;
}

/*!
 * \brief Implementation of StandardSudokuSearchTree class
 */
class StandardSudokuSearchTreePrivate
{
public:
    StandardSudokuSearchTreePrivate(std::array<std::array<int, 9>, 9> &board, bool findAllSolutions)
        : m_board(board), m_findAllSolutions(findAllSolutions), m_error(false)
    {}

    std::array<std::array<int, 9>, 9> &m_board;
    QList<QSharedPointer<Node> > m_headNodes;
    //! List of nodes that reached a solution
    QLinkedList<QSharedPointer<Node> > m_finishedNodes;
    bool m_findAllSolutions;
    bool m_error;
};


StandardSudokuSearchTree::StandardSudokuSearchTree(std::array<std::array<int, 9>, 9> &board, bool findAllSolutions)
    : d_ptr(new StandardSudokuSearchTreePrivate(board, findAllSolutions))
{
}

StandardSudokuSearchTree::~StandardSudokuSearchTree()
{
}

void StandardSudokuSearchTree::search_MultiThread()
{
    Q_D(StandardSudokuSearchTree);

    // Cleanup internals and get ready for new search
    d->m_finishedNodes.clear();
    d->m_headNodes.clear();
    d->m_error = false;

    // Initialize head index(es)
    if (d->m_board[0][0] == EMPTY_CELL_VALUE) {
        const QVector<int> availableValues = StandardSudokuSolver::availableValues(d->m_board, 0, 0);
        if (availableValues.size() == 0) {
            // Board is invalid
            d->m_error = true;
            return;
        }

        for (int x = 0; x < availableValues.size(); ++x) {
            QSharedPointer<Node> head(new Node);
            head->index.row = 0;
            head->index.column = 0;
            head->value = availableValues.at(x);

            d->m_headNodes.append(head);
        }
    } else {
        QSharedPointer<Node> head(new Node);
        head->index.row = 0;
        head->index.column = 0;
        head->value = d->m_board[0][0];

        d->m_headNodes.append(head);
    }

    // Create nodes list with length idealThreadCount
    const int idealThreadCount = QThread::idealThreadCount();

    std::array<std::array<int, 9>, 9> testBoard;
    QAtomicInt found;

    // Use stack instead of recursive functions to prevent stack overflow
    QStack<MultiThreadSearchNode> nodesList;
    for (int x = 0; x < d->m_headNodes.size(); ++x) {
        testBoard = d->m_board;
        fillBoardTillHead(testBoard, d->m_headNodes[x]);

        MultiThreadSearchNode node;
        node.node = d->m_headNodes[x];
        node.board = testBoard;
        node.found = &found;
        node.cancelWhenFound = !d->m_findAllSolutions;
        nodesList.push(node);
    }

    if (nodesList.size() < idealThreadCount) {
        int iterationCount = 0;
        while (!nodesList.empty() && iterationCount++ < 100) {
            QSharedPointer<Node> currentNode = nodesList.pop().node;

            int childNodeColumn;
            int childNodeRow;

            if (currentNode->index.column == 8) {
                if (currentNode->index.row == 8) {
                    d->m_finishedNodes.append(currentNode);
                    break;
                } else {
                    childNodeColumn = 0;
                    childNodeRow = currentNode->index.row + 1;
                }
            } else {
                childNodeColumn = currentNode->index.column + 1;
                childNodeRow = currentNode->index.row;
            }

            if (d->m_board[childNodeRow][childNodeColumn] == EMPTY_CELL_VALUE) {
                testBoard = d->m_board;
                fillBoardTillHead(testBoard, currentNode);
                const QVector<int> availableValues = StandardSudokuSolver::availableValues(testBoard,
                                                                                           childNodeRow,
                                                                                           childNodeColumn);
                for (int x = 0; x < availableValues.size(); ++x) {
                    QSharedPointer<Node> child(new Node);
                    child->parentNode = currentNode;
                    child->index.column = childNodeColumn;
                    child->index.row = childNodeRow;
                    child->value = availableValues.at(x);
                    currentNode->subNodes.append(child);
                }
            } else {
                QSharedPointer<Node> child(new Node);
                child->parentNode = currentNode;
                child->index.column = childNodeColumn;
                child->index.row = childNodeRow;
                child->value = d->m_board[childNodeRow][childNodeColumn];
                currentNode->subNodes.append(child);
            }

            // Append child nodes to stack
            for (auto it = currentNode->subNodes.begin(); it != currentNode->subNodes.end(); ++it) {
                testBoard = d->m_board;
                fillBoardTillHead(testBoard, *it);

                MultiThreadSearchNode node;
                node.node = *it;
                node.board = testBoard;
                node.found = &found;
                node.cancelWhenFound = !d->m_findAllSolutions;
                nodesList.push(node);
            }

            if (nodesList.size() >= idealThreadCount)
                break;
        }
    }

    if (d->m_findAllSolutions || d->m_finishedNodes.size() == 0) {
        found.store(0);
        QList<QLinkedList<QSharedPointer<Node> > > result = QtConcurrent::blockingMapped(nodesList.toList(),
                                                                                         searchNode);
        for (int x = 0; x < result.size(); ++x)
            d->m_finishedNodes << result[x];
    }

    if (d->m_finishedNodes.size() > 0)
        fillBoardTillHead(d->m_board, d->m_finishedNodes.first());
}

void StandardSudokuSearchTree::search_singleThread()
{
    Q_D(StandardSudokuSearchTree);

    // Cleanup internals and get ready for new search
    d->m_finishedNodes.clear();
    d->m_headNodes.clear();
    d->m_error = false;

    // Initialize head index(es)
    if (d->m_board[0][0] == EMPTY_CELL_VALUE) {
        const QVector<int> availableValues = StandardSudokuSolver::availableValues(d->m_board, 0, 0);
        if (availableValues.size() == 0) {
            // Board is invalid
            d->m_error = true;
            return;
        }

        for (int x = 0; x < availableValues.size(); ++x) {
            QSharedPointer<Node> head(new Node);
            head->index.row = 0;
            head->index.column = 0;
            head->value = availableValues.at(x);

            d->m_headNodes.append(head);
        }
    } else {
        QSharedPointer<Node> head(new Node);
        head->index.row = 0;
        head->index.column = 0;
        head->value = d->m_board[0][0];

        d->m_headNodes.append(head);
    }

    // Use stack instead of recursive functions to prevent stack overflow
    QStack<QSharedPointer<Node> > nodesList;
    for (int x = 0; x < d->m_headNodes.size(); ++x)
        nodesList.push(d->m_headNodes[x]);

    std::array<std::array<int, 9>, 9> testBoard;
    while (!nodesList.empty()) {
        QSharedPointer<Node> currentNode = nodesList.pop();

        int childNodeColumn;
        int childNodeRow;

        if (currentNode->index.column == 8) {
            if (currentNode->index.row == 8) {
                d->m_finishedNodes.append(currentNode);
                if (!d->m_findAllSolutions)
                    break;
                continue;
            } else {
                childNodeColumn = 0;
                childNodeRow = currentNode->index.row + 1;
            }
        } else {
            childNodeColumn = currentNode->index.column + 1;
            childNodeRow = currentNode->index.row;
        }

        if (d->m_board[childNodeRow][childNodeColumn] == EMPTY_CELL_VALUE) {
            testBoard = d->m_board;
            fillBoardTillHead(testBoard, currentNode);
            const QVector<int> availableValues = StandardSudokuSolver::availableValues(testBoard,
                                                                                       childNodeRow,
                                                                                       childNodeColumn);
            for (int x = 0; x < availableValues.size(); ++x) {
                QSharedPointer<Node> child(new Node);
                child->parentNode = currentNode;
                child->index.column = childNodeColumn;
                child->index.row = childNodeRow;
                child->value = availableValues.at(x);
                currentNode->subNodes.append(child);
            }
        } else {
            QSharedPointer<Node> child(new Node);
            child->parentNode = currentNode;
            child->index.column = childNodeColumn;
            child->index.row = childNodeRow;
            child->value = d->m_board[childNodeRow][childNodeColumn];
            currentNode->subNodes.append(child);
        }

        // Append child nodes to stack
        for (auto it = currentNode->subNodes.begin(); it != currentNode->subNodes.end(); ++it)
            nodesList.push(*it);
    }
}

bool StandardSudokuSearchTree::isSuccessfull()
{
    Q_D(const StandardSudokuSearchTree);
    return d->m_finishedNodes.size() > 0;
}

int StandardSudokuSearchTree::finishedNodesCount() const
{
    Q_D(const StandardSudokuSearchTree);
    return d->m_finishedNodes.size();
}
