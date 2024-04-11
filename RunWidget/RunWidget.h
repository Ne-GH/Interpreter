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

    int only_read_length_ = 0;

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

class RunWidget : public QWidget {
    Q_OBJECT

public:
    static RunWidget& GetInstance();
    RunResult &GetRunResult();
    ~RunWidget();
    void Clear();

private:
    explicit RunWidget(QWidget *parent = nullptr);
    RunWidget(const RunWidget&);
    RunWidget& operator = (const RunWidget&);

    Ui::RunWidget *ui;
    RunResult *run_result_;

};


#define RUNRESULT RunWidget::GetInstance().GetRunResult()
#endif // RUNWIDGET_H
