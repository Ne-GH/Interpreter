#include "RunWidget.h"
#include "ui_RunWidget.h"

#include <QGridLayout>

RunResult::RunResult(QWidget *parent) :
        QTextEdit(parent) {

}

int RunResult::GetLength() const {
    
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
