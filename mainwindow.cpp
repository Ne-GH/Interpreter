#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    auto p = ui;
    setWindowTitle("解释器");
    connect(ui->run_action, &QAction::triggered, [=] {
        _run_window.Show();
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

