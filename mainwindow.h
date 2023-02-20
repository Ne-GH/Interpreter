#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Logs/Log.h"
#include "RunResult/RunResult.h"
#include "File/File.h"

#include <QMainWindow>
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

    RunResult _run_window;
    File* _file;
    
    Log _log;
    
};
#endif // MAINWINDOW_H
