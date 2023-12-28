/********************************************************************************
** Form generated from reading UI file 'Log.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOG_H
#define UI_LOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_Log
{
public:
    QTextEdit *log_edit;

    void setupUi(QDialog *Log)
    {
        if (Log->objectName().isEmpty())
            Log->setObjectName("Log");
        Log->resize(533, 450);
        log_edit = new QTextEdit(Log);
        log_edit->setObjectName("log_edit");
        log_edit->setGeometry(QRect(0, 0, 531, 451));

        retranslateUi(Log);

        QMetaObject::connectSlotsByName(Log);
    } // setupUi

    void retranslateUi(QDialog *Log)
    {
        Log->setWindowTitle(QCoreApplication::translate("Log", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Log: public Ui_Log {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOG_H
