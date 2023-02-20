#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./Debug.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("解释器");
    setCentralWidget(ui->file_edit);


    _file = new File(QApplication::activeWindow());

    connect(ui->create_file, &QAction::triggered, [=] {
        auto create_path = _file->Create();
		_file->Read(create_path);
    });
    connect(ui->open_file, &QAction::triggered, [=] {
        _file->Open();
        ui->file_edit->setText(_file->GetContent().data());
    });

    connect(ui->save_file, &QAction::triggered, [=] {
        std::string string = ui->file_edit->toPlainText().toStdString();
        _file->Save(string);
    });
    
    connect(ui->close_file, &QAction::triggered, [=] {
        _file->Close();
        ui->file_edit->setText("");
    
    });
    connect(ui->run_action, &QAction::triggered, [=] {
        _run_window.Show();
    });
    
   
    
    ui->create_file->setShortcut(QKeySequence("Ctrl+N"));
    ui->open_file->setShortcut(QKeySequence("Ctrl+O"));
    ui->save_file->setShortcut(QKeySequence("Ctrl+S"));
    ui->close_file->setShortcut(QKeySequence("Ctrl+W"));

}

MainWindow::~MainWindow()
{
    delete ui;
}

