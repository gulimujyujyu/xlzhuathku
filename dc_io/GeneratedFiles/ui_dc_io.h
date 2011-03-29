/********************************************************************************
** Form generated from reading UI file 'dc_io.ui'
**
** Created: Tue Mar 29 15:42:03 2011
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DC_IO_H
#define UI_DC_IO_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dc_ioClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *dc_ioClass)
    {
        if (dc_ioClass->objectName().isEmpty())
            dc_ioClass->setObjectName(QString::fromUtf8("dc_ioClass"));
        dc_ioClass->resize(600, 400);
        menuBar = new QMenuBar(dc_ioClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        dc_ioClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(dc_ioClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        dc_ioClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(dc_ioClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        dc_ioClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(dc_ioClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        dc_ioClass->setStatusBar(statusBar);

        retranslateUi(dc_ioClass);

        QMetaObject::connectSlotsByName(dc_ioClass);
    } // setupUi

    void retranslateUi(QMainWindow *dc_ioClass)
    {
        dc_ioClass->setWindowTitle(QApplication::translate("dc_ioClass", "dc_io", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class dc_ioClass: public Ui_dc_ioClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DC_IO_H
