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
    void mousePressEvent(QMouseEvent *)       { /* Ignore */ }
    void mouseDoubleClickEvent(QMouseEvent *) { /* Ignore */ }
    void mouseMoveEvent(QMouseEvent *)        { /* Ignore */ }
    void mouseReleaseEvent(QMouseEvent *)     { /* Ignore */ }
public:
    explicit RunResult(QWidget *parent = nullptr);

    ~RunResult() = default;

    int GetLength() const ;


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
