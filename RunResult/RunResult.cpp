#include "runresult.h"
#include "ui_runresult.h"
#include <QEvent.h>
#include "../Debug.h"
RunResult::RunResult(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RunResult)
{
    ui->setupUi(this);
    ui->run_edit->setReadOnly(true);
}

RunResult::~RunResult()
{
    delete ui;
}


void RunResult::Show() {
    show();
    activateWindow();
}

void RunResult::Hide() {
    hide();
}

void RunResult::keyPressEvent(QKeyEvent* event) {
    Mes("keyPress");
    if (event->key() == Qt::Key_Return) {
        QString input = ui->run_edit->toPlainText();
        ui->run_edit->append(input);
        ui->run_edit->moveCursor(QTextCursor::End);
    }

}