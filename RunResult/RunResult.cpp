#include "runresult.h"
#include "ui_runresult.h"

RunResult::RunResult(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RunResult)
{
    ui->setupUi(this);
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