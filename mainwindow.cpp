#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./Debug.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    setWindowTitle("解释器");
    

    connect(ui->open_file, &QAction::triggered, [=] {
        
    });

    connect(ui->save_file, &QAction::triggered, [=] {

    });
    
    connect(ui->close_file, &QAction::triggered, [=] {
    
    });
    connect(ui->run_action, &QAction::triggered, [=] {
        _run_window.Show();
    });
    
    setCentralWidget(ui->file_edit);
    
    ui->open_file->setShortcut(QKeySequence("Ctrl+O"));
    

}

MainWindow::~MainWindow()
{
    delete ui;
}

