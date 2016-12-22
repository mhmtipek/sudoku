#ifndef SUDOKUBOARDCONTROL_H
#define SUDOKUBOARDCONTROL_H

#include <QObject>

class SudokuBoardLinearModel;

class SudokuBoardControl : public QObject
{
    Q_OBJECT
public:
    explicit SudokuBoardControl(SudokuBoardLinearModel *sudokuBoardLinearModel, QObject *parent = 0);

signals:
    void finishedSuccessfully();

private slots:
    void checkForFinished();

private:
    SudokuBoardLinearModel *m_sudokuBoardLinearModel;

//    SudokuBoardModelProxy m_proxyModel;
};

#endif // SUDOKUBOARDCONTROL_H
