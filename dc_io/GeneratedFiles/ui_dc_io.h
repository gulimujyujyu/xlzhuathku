/********************************************************************************
** Form generated from reading UI file 'dc_io.ui'
**
** Created: Sun Apr 3 17:45:13 2011
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
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>
#include <xldisplay.h>

QT_BEGIN_NAMESPACE

class Ui_dc_ioClass
{
public:
    QAction *actionConnect;
    QAction *actionExit;
    QAction *actionRaw;
    QAction *actionRegistered;
    QAction *action3D;
    QAction *actionROI;
    QAction *actionROD;
    QAction *action1;
    QAction *action05;
    QAction *action2;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    XlDisplay *Display;
    QPushButton *PlayButton;
    QPushButton *CaptureButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *RecordButton;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menu_Set;
    QMenu *menuScale;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *dc_ioClass)
    {
        if (dc_ioClass->objectName().isEmpty())
            dc_ioClass->setObjectName(QString::fromUtf8("dc_ioClass"));
        dc_ioClass->resize(824, 629);
        actionConnect = new QAction(dc_ioClass);
        actionConnect->setObjectName(QString::fromUtf8("actionConnect"));
        actionExit = new QAction(dc_ioClass);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionRaw = new QAction(dc_ioClass);
        actionRaw->setObjectName(QString::fromUtf8("actionRaw"));
        actionRegistered = new QAction(dc_ioClass);
        actionRegistered->setObjectName(QString::fromUtf8("actionRegistered"));
        action3D = new QAction(dc_ioClass);
        action3D->setObjectName(QString::fromUtf8("action3D"));
        actionROI = new QAction(dc_ioClass);
        actionROI->setObjectName(QString::fromUtf8("actionROI"));
        actionROD = new QAction(dc_ioClass);
        actionROD->setObjectName(QString::fromUtf8("actionROD"));
        action1 = new QAction(dc_ioClass);
        action1->setObjectName(QString::fromUtf8("action1"));
        action05 = new QAction(dc_ioClass);
        action05->setObjectName(QString::fromUtf8("action05"));
        action2 = new QAction(dc_ioClass);
        action2->setObjectName(QString::fromUtf8("action2"));
        centralWidget = new QWidget(dc_ioClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        Display = new XlDisplay(centralWidget);
        Display->setObjectName(QString::fromUtf8("Display"));

        gridLayout->addWidget(Display, 0, 0, 1, 5);

        PlayButton = new QPushButton(centralWidget);
        PlayButton->setObjectName(QString::fromUtf8("PlayButton"));
        PlayButton->setCheckable(true);

        gridLayout->addWidget(PlayButton, 1, 0, 1, 1);

        CaptureButton = new QPushButton(centralWidget);
        CaptureButton->setObjectName(QString::fromUtf8("CaptureButton"));

        gridLayout->addWidget(CaptureButton, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 3, 1, 1);

        RecordButton = new QPushButton(centralWidget);
        RecordButton->setObjectName(QString::fromUtf8("RecordButton"));
        RecordButton->setCheckable(true);

        gridLayout->addWidget(RecordButton, 1, 2, 1, 1);

        dc_ioClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(dc_ioClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 824, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menu_Set = new QMenu(menuBar);
        menu_Set->setObjectName(QString::fromUtf8("menu_Set"));
        menuScale = new QMenu(menu_Set);
        menuScale->setObjectName(QString::fromUtf8("menuScale"));
        dc_ioClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(dc_ioClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        dc_ioClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menu_Set->menuAction());
        menuFile->addAction(actionConnect);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuView->addAction(actionRaw);
        menuView->addAction(actionRegistered);
        menuView->addAction(action3D);
        menu_Set->addAction(actionROI);
        menu_Set->addAction(actionROD);
        menu_Set->addSeparator();
        menu_Set->addAction(menuScale->menuAction());
        menuScale->addAction(action2);
        menuScale->addAction(action1);
        menuScale->addAction(action05);

        retranslateUi(dc_ioClass);

        QMetaObject::connectSlotsByName(dc_ioClass);
    } // setupUi

    void retranslateUi(QMainWindow *dc_ioClass)
    {
        dc_ioClass->setWindowTitle(QApplication::translate("dc_ioClass", "dc_io", 0, QApplication::UnicodeUTF8));
        actionConnect->setText(QApplication::translate("dc_ioClass", "&Connect", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("dc_ioClass", "Exit", 0, QApplication::UnicodeUTF8));
        actionRaw->setText(QApplication::translate("dc_ioClass", "&Raw", 0, QApplication::UnicodeUTF8));
        actionRegistered->setText(QApplication::translate("dc_ioClass", "R&egistered", 0, QApplication::UnicodeUTF8));
        action3D->setText(QApplication::translate("dc_ioClass", "3&D", 0, QApplication::UnicodeUTF8));
        actionROI->setText(QApplication::translate("dc_ioClass", "RO&I", 0, QApplication::UnicodeUTF8));
        actionROD->setText(QApplication::translate("dc_ioClass", "RO&D", 0, QApplication::UnicodeUTF8));
        action1->setText(QApplication::translate("dc_ioClass", "1", 0, QApplication::UnicodeUTF8));
        action05->setText(QApplication::translate("dc_ioClass", "0.5", 0, QApplication::UnicodeUTF8));
        action2->setText(QApplication::translate("dc_ioClass", "2", 0, QApplication::UnicodeUTF8));
        PlayButton->setText(QApplication::translate("dc_ioClass", "Play", 0, QApplication::UnicodeUTF8));
        CaptureButton->setText(QApplication::translate("dc_ioClass", "Capture", 0, QApplication::UnicodeUTF8));
        RecordButton->setText(QApplication::translate("dc_ioClass", "Record", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("dc_ioClass", "&File", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("dc_ioClass", "&View", 0, QApplication::UnicodeUTF8));
        menu_Set->setTitle(QApplication::translate("dc_ioClass", "&Set", 0, QApplication::UnicodeUTF8));
        menuScale->setTitle(QApplication::translate("dc_ioClass", "&Scale", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class dc_ioClass: public Ui_dc_ioClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DC_IO_H
