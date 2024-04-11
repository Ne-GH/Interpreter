#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Logs/Log.h"
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), file_(this)
{
    ui->setupUi(this);


    setWindowTitle("解释器");
    setWindowIcon(QIcon(":/Resource/interpreter.png"));

    setCentralWidget(ui->file_edit);

    connect(ui->create_file, &QAction::triggered, [=] {
        auto create_path = file_.Create();
		file_.Read(create_path);
    });
    connect(ui->open_file, &QAction::triggered, [=] {
        file_.Open();
        ui->file_edit->setText(file_.GetContent().data());
    });

    connect(ui->save_file, &QAction::triggered, [=] {
        std::string string = ui->file_edit->toPlainText().toStdString();
        file_.Save(string);
    });
    
    connect(ui->close_file, &QAction::triggered, [=] {
        file_.Close();
        ui->file_edit->setText("");
    
    });
    connect(ui->run_action, &QAction::triggered, [&] {
        RunWidget::GetInstance().show();
        RunWidget::GetInstance().Clear();
        interpreter_.SetMod(Interpreter::RUN);
        interpreter_.Run(file_.GetContent());

    });
    connect(ui->asm_action,&QAction::triggered,[&] {
        RunWidget::GetInstance().show();
        RunWidget::GetInstance().Clear();
        interpreter_.SetMod(Interpreter::ASM);
        interpreter_.Run(file_.GetContent());
    });
    connect(ui->log_action, &QAction::triggered, [=] {
        LOG.Show();
    });
    

    //interpreter_ = std::make_shared<Interpreter>();
    
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

