#include "standardsudokugamecontrol.h"

#include "standardsudokusolver.h"

#include <array>

// TODO: Remove later
#include <QDebug>

StandardSudokuGameControl::StandardSudokuGameControl(QObject *parent) :
    AbstractGameControl(parent),
    m_model(nullptr),
    m_difficulty(1),
    m_finishTime(0),
    m_previousElapsedTimes(0)
{
    // Init board creator
    connect(&m_boardCreator, SIGNAL(progressChanged()),
            this, SIGNAL(initialTableCreationProgressChanged()));

    connect(&m_boardCreator, SIGNAL(finished()),
            this, SLOT(handleBoardCreatorFinished()));

    connect(&m_boardCreator, SIGNAL(progressTextChanged()),
            this, SIGNAL(initialTableCreationProgressTextChanged()));
}

StandardSudokuGameControl::~StandardSudokuGameControl()
{

}

void StandardSudokuGameControl::setModel(SudokuBoardModel *model)
{
    if (m_model != nullptr) {
        disconnect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                   this, SLOT(handleDataChanged(QModelIndex,QModelIndex,QVector<int>)));
    }

    m_model = model;

    if (m_model == nullptr)
        return;

    m_model->makeAllCellsUneditable();

    connect(m_model, SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&,const QVector<int>&)),
            this, SLOT(handleDataChanged(const QModelIndex&,const QModelIndex&,const QVector<int>&)));
}

int StandardSudokuGameControl::initialTableCreationProgress()
{
    return m_boardCreator.progress();
}

QString StandardSudokuGameControl::initialTableCreationProgressText()
{
    return m_boardCreator.progressText();
}

int StandardSudokuGameControl::difficulty() const
{
    return m_difficulty;
}

void StandardSudokuGameControl::setDifficulty(int value)
{
    if (value < 1 || value > 4)
        return;

    m_difficulty = value;
}

void StandardSudokuGameControl::start()
{
    m_finishTime = 0;
    m_previousElapsedTimes = 0;
    m_elapsedTime.start();
}

int StandardSudokuGameControl::elapsedTime() const
{
    return m_elapsedTime.elapsed();
}

int StandardSudokuGameControl::finishTime() const
{
    return m_finishTime;
}

void StandardSudokuGameControl::resumeGameTime()
{
    m_elapsedTime.restart();
    qDebug() << "Resuming game time";
}

void StandardSudokuGameControl::pauseGameTime()
{
    m_previousElapsedTimes += m_elapsedTime.elapsed();
    qDebug() << "Paused game time";
}

void StandardSudokuGameControl::handleDataChanged(const QModelIndex &topLeft,
                                                  const QModelIndex &bottomRight,
                                                  const QVector<int> &roles)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)

    if (roles.size() == 0)
        return;

    const int role = roles.at(0);

    if (role != SudokuBoardModel::ValueRole)
        return;

    if (StandardSudokuSolver::isBoardFinished(m_model->m_dataGrid)) {
        m_finishTime = m_elapsedTime.elapsed() + m_previousElapsedTimes;
        emit gameFinished();
        return;
    }
}

void StandardSudokuGameControl::handleBoardCreatorFinished()
{
    const std::array<std::array<int, 9>, 9> & result = m_boardCreator.result();

    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            m_model->setData(row, col, result[row][col]);
        }
    }

    m_model->makeEmptyCellsEditable();

    emit creatingInitialTableFinished();
}

void StandardSudokuGameControl::createInitialTable()
{
    qDebug() << "Creating initial table with difficulty" << m_difficulty;
    m_boardCreator.setDifficulty(m_difficulty);
    m_boardCreator.start();
}
