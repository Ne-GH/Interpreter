/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *run_action;
    QAction *asm_action;
    QAction *open_file;
    QAction *save_file;
    QAction *close_file;
    QAction *create_file;
    QAction *log_action;
    QWidget *centralwidget;
    QTextEdit *file_edit;
    QMenuBar *menuBar;
    QMenu *operator_menu;
    QMenu *file_menu;
    QMenu *log_menu;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(568, 630);
        run_action = new QAction(MainWindow);
        run_action->setObjectName("run_action");
        asm_action = new QAction(MainWindow);
        asm_action->setObjectName("asm_action");
        open_file = new QAction(MainWindow);
        open_file->setObjectName("open_file");
        save_file = new QAction(MainWindow);
        save_file->setObjectName("save_file");
        close_file = new QAction(MainWindow);
        close_file->setObjectName("close_file");
        create_file = new QAction(MainWindow);
        create_file->setObjectName("create_file");
        log_action = new QAction(MainWindow);
        log_action->setObjectName("log_action");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        file_edit = new QTextEdit(centralwidget);
        file_edit->setObjectName("file_edit");
        file_edit->setGeometry(QRect(9, 9, 256, 192));
        MainWindow->setCentralWidget(centralwidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 568, 23));
        operator_menu = new QMenu(menuBar);
        operator_menu->setObjectName("operator_menu");
        file_menu = new QMenu(menuBar);
        file_menu->setObjectName("file_menu");
        log_menu = new QMenu(menuBar);
        log_menu->setObjectName("log_menu");
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(file_menu->menuAction());
        menuBar->addAction(operator_menu->menuAction());
        menuBar->addAction(log_menu->menuAction());
        operator_menu->addAction(run_action);
        operator_menu->addAction(asm_action);
        file_menu->addAction(create_file);
        file_menu->addAction(open_file);
        file_menu->addAction(save_file);
        file_menu->addAction(close_file);
        log_menu->addAction(log_action);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        run_action->setText(QCoreApplication::translate("MainWindow", "\350\277\220\350\241\214", nullptr));
        asm_action->setText(QCoreApplication::translate("MainWindow", "\346\261\207\347\274\226", nullptr));
        open_file->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200", nullptr));
        save_file->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
        close_file->setText(QCoreApplication::translate("MainWindow", "\345\205\263\351\227\255", nullptr));
        create_file->setText(QCoreApplication::translate("MainWindow", "\345\210\233\345\273\272", nullptr));
        log_action->setText(QCoreApplication::translate("MainWindow", "\346\227\245\345\277\227", nullptr));
        file_edit->setDocumentTitle(QString());
        operator_menu->setTitle(QCoreApplication::translate("MainWindow", "\346\223\215\344\275\234", nullptr));
        file_menu->setTitle(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266", nullptr));
        log_menu->setTitle(QCoreApplication::translate("MainWindow", "\346\227\245\345\277\227", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
