/********************************************************************************
** Form generated from reading UI file 'dc_viewer.ui'
**
** Created: Tue Apr 12 19:46:38 2011
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DC_VIEWER_H
#define UI_DC_VIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGraphicsView>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QTreeView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dc_viewerClass
{
public:
    QAction *actionSet_Root;
    QAction *action_Exit;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QTreeView *fileTreeView;
    QLabel *colorLabel;
    QLabel *depthLabel;
    QLabel *normalLabel;
    QLabel *curvatureLabel;
    QGraphicsView *graphicsView;
    QLabel *normalLabel_2;
    QLabel *curvatureLabel_2;
    QLabel *edgeLabel;
    QLabel *tempLabel;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *dc_viewerClass)
    {
        if (dc_viewerClass->objectName().isEmpty())
            dc_viewerClass->setObjectName(QString::fromUtf8("dc_viewerClass"));
        dc_viewerClass->resize(536, 301);
        actionSet_Root = new QAction(dc_viewerClass);
        actionSet_Root->setObjectName(QString::fromUtf8("actionSet_Root"));
        action_Exit = new QAction(dc_viewerClass);
        action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
        centralWidget = new QWidget(dc_viewerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        fileTreeView = new QTreeView(centralWidget);
        fileTreeView->setObjectName(QString::fromUtf8("fileTreeView"));

        gridLayout->addWidget(fileTreeView, 0, 0, 5, 1);

        colorLabel = new QLabel(centralWidget);
        colorLabel->setObjectName(QString::fromUtf8("colorLabel"));

        gridLayout->addWidget(colorLabel, 0, 1, 1, 1);

        depthLabel = new QLabel(centralWidget);
        depthLabel->setObjectName(QString::fromUtf8("depthLabel"));

        gridLayout->addWidget(depthLabel, 0, 2, 1, 1);

        normalLabel = new QLabel(centralWidget);
        normalLabel->setObjectName(QString::fromUtf8("normalLabel"));

        gridLayout->addWidget(normalLabel, 1, 1, 1, 1);

        curvatureLabel = new QLabel(centralWidget);
        curvatureLabel->setObjectName(QString::fromUtf8("curvatureLabel"));

        gridLayout->addWidget(curvatureLabel, 1, 2, 1, 1);

        graphicsView = new QGraphicsView(centralWidget);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));

        gridLayout->addWidget(graphicsView, 4, 1, 1, 2);

        normalLabel_2 = new QLabel(centralWidget);
        normalLabel_2->setObjectName(QString::fromUtf8("normalLabel_2"));

        gridLayout->addWidget(normalLabel_2, 2, 2, 1, 1);

        curvatureLabel_2 = new QLabel(centralWidget);
        curvatureLabel_2->setObjectName(QString::fromUtf8("curvatureLabel_2"));

        gridLayout->addWidget(curvatureLabel_2, 2, 1, 1, 1);

        edgeLabel = new QLabel(centralWidget);
        edgeLabel->setObjectName(QString::fromUtf8("edgeLabel"));

        gridLayout->addWidget(edgeLabel, 3, 1, 1, 1);

        tempLabel = new QLabel(centralWidget);
        tempLabel->setObjectName(QString::fromUtf8("tempLabel"));

        gridLayout->addWidget(tempLabel, 3, 2, 1, 1);

        dc_viewerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(dc_viewerClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 536, 21));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        dc_viewerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(dc_viewerClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        dc_viewerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(dc_viewerClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        dc_viewerClass->setStatusBar(statusBar);

        menuBar->addAction(menu_File->menuAction());
        menu_File->addAction(actionSet_Root);
        menu_File->addSeparator();
        menu_File->addAction(action_Exit);

        retranslateUi(dc_viewerClass);

        QMetaObject::connectSlotsByName(dc_viewerClass);
    } // setupUi

    void retranslateUi(QMainWindow *dc_viewerClass)
    {
        dc_viewerClass->setWindowTitle(QApplication::translate("dc_viewerClass", "dc_viewer", 0, QApplication::UnicodeUTF8));
        actionSet_Root->setText(QApplication::translate("dc_viewerClass", "Set &Root", 0, QApplication::UnicodeUTF8));
        action_Exit->setText(QApplication::translate("dc_viewerClass", "&Exit", 0, QApplication::UnicodeUTF8));
        colorLabel->setText(QApplication::translate("dc_viewerClass", "Color", 0, QApplication::UnicodeUTF8));
        depthLabel->setText(QApplication::translate("dc_viewerClass", "Depth", 0, QApplication::UnicodeUTF8));
        normalLabel->setText(QApplication::translate("dc_viewerClass", "Normal", 0, QApplication::UnicodeUTF8));
        curvatureLabel->setText(QApplication::translate("dc_viewerClass", "Curvature", 0, QApplication::UnicodeUTF8));
        normalLabel_2->setText(QApplication::translate("dc_viewerClass", "Normal", 0, QApplication::UnicodeUTF8));
        curvatureLabel_2->setText(QApplication::translate("dc_viewerClass", "Curvature", 0, QApplication::UnicodeUTF8));
        edgeLabel->setText(QApplication::translate("dc_viewerClass", "Edge", 0, QApplication::UnicodeUTF8));
        tempLabel->setText(QApplication::translate("dc_viewerClass", "N/A", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("dc_viewerClass", "&File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class dc_viewerClass: public Ui_dc_viewerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DC_VIEWER_H
