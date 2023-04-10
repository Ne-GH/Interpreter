#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Logs/Log.h"
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), _file(this)
{
    ui->setupUi(this);


    setWindowTitle("解释器");
    setWindowIcon(QIcon(":/Resource/interpreter.png"));

    setCentralWidget(ui->file_edit);

    connect(ui->create_file, &QAction::triggered, [=] {
        auto create_path = _file.Create();
		_file.Read(create_path);
    });
    connect(ui->open_file, &QAction::triggered, [=] {
        _file.Open();
        ui->file_edit->setText(_file.GetContent().data());
    });

    connect(ui->save_file, &QAction::triggered, [=] {
        std::string string = ui->file_edit->toPlainText().toStdString();
        _file.Save(string);
    });
    
    connect(ui->close_file, &QAction::triggered, [=] {
        _file.Close();
        ui->file_edit->setText("");
    
    });
    connect(ui->run_action, &QAction::triggered, [=] {
        RunWidget::GetInstance().show();
        RunWidget::GetInstance().Clear();
        _interpreter->SetMod(Interpreter::RUN);
        _interpreter->Run(_file.GetContent());

    });
    connect(ui->asm_action,&QAction::triggered,[=] {
        RunWidget::GetInstance().show();
        RunWidget::GetInstance().Clear();
        _interpreter->SetMod(Interpreter::ASM);
        _interpreter->Run(_file.GetContent());
    });
    connect(ui->log_action, &QAction::triggered, [=] {
        LOG.Show();
    });
    

    _interpreter = std::make_shared<Interpreter>();
    
    ui->create_file->setShortcut(QKeySequence("Ctrl+N"));
    ui->open_file->setShortcut(QKeySequence("Ctrl+O"));
    ui->save_file->setShortcut(QKeySequence("Ctrl+S"));
    ui->close_file->setShortcut(QKeySequence("Ctrl+W"));

    LOG.AddLog("绑定快捷键完成");

}

MainWindow::~MainWindow()
{

    delete ui;
}

