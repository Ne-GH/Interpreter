#include "RunWidget.h"
#include "ui_RunWidget.h"
#include <QIcon>
#include <QKeyEvent>
#include <QGridLayout>
#include <QTextBlock>

// 运行结果编辑界面
RunResult::RunResult(QWidget *parent) :
        QTextEdit(parent) {
    setLineWrapMode(NoWrap);
    
}

void RunResult::Output(std::string out_message) {
    moveCursor(QTextCursor::End);
    insertPlainText(out_message.c_str());
    auto cur_cursor = textCursor();
    _only_read_length = cur_cursor.position() - cur_cursor.block().position();
}
std::string RunResult::GetInput() {
    auto cur_cursor = textCursor();
    auto cur_block = cur_cursor.block();
    std::string input = cur_block.text().toStdString();
    input.erase(0,_only_read_length);
    insertPlainText("\n");
    return input;
}
void RunResult::SetCursorPos(int pos) {
    auto cur_cursor = textCursor();
    auto cur_block = cur_cursor.block();
    cur_cursor.setPosition(pos + cur_block.position());
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
        case Qt::Key_Return:
        case Qt::Key_Enter:
            emit Input();
            break;

        default:
            QTextEdit::keyPressEvent(event);
            break;
    }
}

void RunWidget::Clear() {
    GetRunResult().clear();
}



// 运行窗口
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
    setWindowTitle("运行窗口");
    setWindowIcon(QIcon(":/Resource/terminal.png"));

    connect(_run_result,&RunResult::Input,[=]{
        _run_result->Output(_run_result->GetInput() ) ;
    });


}

RunWidget::~RunWidget() {
    delete ui;
}

RunResult &RunWidget::GetRunResult() {
    return *_run_result;
}


RunWidget &RunWidget::GetInstance() {
    static std::unique_ptr<RunWidget> _instance;
    if (_instance == nullptr) {
        _instance.reset(new RunWidget());
    }
    return  *_instance;
}
