#ifndef RUNRESULT_H
#define RUNRESULT_H

#include <QWidget>

namespace Ui {
class RunResult;
}

class RunResult : public QWidget
{
    Q_OBJECT

public:
    explicit RunResult(QWidget *parent = nullptr);
    ~RunResult();

    void Show();
    void Hide();
private slots:
    void keyPressEvent(QKeyEvent* event);


private:
    Ui::RunResult *ui;
};

#endif // RUNRESULT_H
