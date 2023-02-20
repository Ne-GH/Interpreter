#include "RunResult.h"
#include "ui_runresult.h"
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
