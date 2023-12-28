/********************************************************************************
** Form generated from reading UI file 'RunWidget.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RUNWIDGET_H
#define UI_RUNWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RunWidget
{
public:

    void setupUi(QWidget *RunWidget)
    {
        if (RunWidget->objectName().isEmpty())
            RunWidget->setObjectName("RunWidget");
        RunWidget->resize(400, 300);

        retranslateUi(RunWidget);

        QMetaObject::connectSlotsByName(RunWidget);
    } // setupUi

    void retranslateUi(QWidget *RunWidget)
    {
        RunWidget->setWindowTitle(QCoreApplication::translate("RunWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RunWidget: public Ui_RunWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RUNWIDGET_H
