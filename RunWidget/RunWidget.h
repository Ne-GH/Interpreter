#ifndef RUNWIDGET_H
#define RUNWIDGET_H

#include <QWidget>
#include <QTextEdit>
namespace Ui {
    class RunWidget;
}


class RunResult : public QTextEdit {
    Q_OBJECT

    // Do not handle other events
    void mousePressEvent(QMouseEvent *)       override { /* Ignore */ }
    void mouseDoubleClickEvent(QMouseEvent *) override { /* Ignore */ }
    void mouseMoveEvent(QMouseEvent *)        override { /* Ignore */ }
    void mouseReleaseEvent(QMouseEvent *)     override { /* Ignore */ }
    void keyPressEvent(QKeyEvent *)           override ;

    int _only_read_length;

public:
    explicit RunResult(QWidget *parent = nullptr);

    ~RunResult() = default;

    void SetCursorPos(int) ;
    int GetCursorPos() const ;


};

class RunWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RunWidget(QWidget *parent = nullptr);
    ~RunWidget();

private:
    Ui::RunWidget *ui;
    RunResult *_run_result;
};

#endif // RUNWIDGET_H
