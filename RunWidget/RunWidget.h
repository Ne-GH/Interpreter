#ifndef RUNWIDGET_H
#define RUNWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <string>
namespace Ui {
    class RunWidget;
}


class RunResult : public QTextEdit {
    Q_OBJECT

    int _only_read_length = 0;

    void mousePressEvent(QMouseEvent *)       override { /* Ignore */ }
    void mouseDoubleClickEvent(QMouseEvent *) override { /* Ignore */ }
    void mouseMoveEvent(QMouseEvent *)        override { /* Ignore */ }
    void mouseReleaseEvent(QMouseEvent *)     override { /* Ignore */ }
    void keyPressEvent(QKeyEvent *)           override ;


    void SetCursorPos(int) ;
    int GetCursorPos() const ;
public:
    explicit RunResult(QWidget *parent = nullptr);

    ~RunResult() = default;


    std::string GetInput();
    void Output(std::string);

signals:
    void Input();


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
