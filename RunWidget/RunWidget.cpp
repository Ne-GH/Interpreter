#include "RunWidget.h"
#include "ui_RunWidget.h"

#include <QKeyEvent>
#include <QGridLayout>
#include <QTextBlock>

RunResult::RunResult(QWidget *parent) :
        QTextEdit(parent) {
    setLineWrapMode(NoWrap);
}

void RunResult::SetCursorPos(int pos) {
    auto cur_cursor = textCursor();
    cur_cursor.setPosition(pos);
    setTextCursor(cur_cursor);
}
int RunResult::GetCursorPos() const {
    auto cur_cursor = textCursor();
    return cur_cursor.position()
        - cur_cursor.block().position();
}
void RunResult::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_Down:
            break;
        case Qt::Key_Left:
        case Qt::Key_Backspace:
            if (GetCursorPos() > _only_read_length) {
                QTextEdit::keyPressEvent(event);
            }
            break;
        case Qt::Key_Home:
            SetCursorPos(_only_read_length);
            break;
        default:
            QTextEdit::keyPressEvent(event);
            break;
    }
}


RunWidget::RunWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RunWidget)
{
    ui->setupUi(this);
    _run_result = new RunResult(this);
    auto layout = new QGridLayout(this);
    layout->addWidget(_run_result);
    layout->setContentsMargins(0,0,0,0);
    setContentsMargins(0,0,0,0);
}

RunWidget::~RunWidget()
{
    delete ui;
}
