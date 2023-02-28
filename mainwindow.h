#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

#include "Logs/Log.h"
#include "File/File.h"
#include "RunWidget/RunWidget.h"
#include "Interpreter/Interpreter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    Log _log;
    File _file;
    RunWidget *_run_widget;
    Interpreter _interpreter;

    
};
#endif // MAINWINDOW_H
