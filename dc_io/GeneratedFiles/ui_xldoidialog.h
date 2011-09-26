/********************************************************************************
** Form generated from reading UI file 'xldoidialog.ui'
**
** Created: Mon Sep 26 12:22:41 2011
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XLDOIDIALOG_H
#define UI_XLDOIDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGraphicsView>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_XlDOIDialogClass
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *label_2;
    QPushButton *cancelButton;
    QLineEdit *minLineEdit;
    QLineEdit *maxLineEdit;
    QGraphicsView *DOIHistogram;
    QPushButton *okButton;

    void setupUi(QDialog *XlDOIDialogClass)
    {
        if (XlDOIDialogClass->objectName().isEmpty())
            XlDOIDialogClass->setObjectName(QString::fromUtf8("XlDOIDialogClass"));
        XlDOIDialogClass->resize(480, 240);
        gridLayout = new QGridLayout(XlDOIDialogClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(XlDOIDialogClass);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_2 = new QLabel(XlDOIDialogClass);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        cancelButton = new QPushButton(XlDOIDialogClass);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        gridLayout->addWidget(cancelButton, 2, 3, 1, 1);

        minLineEdit = new QLineEdit(XlDOIDialogClass);
        minLineEdit->setObjectName(QString::fromUtf8("minLineEdit"));

        gridLayout->addWidget(minLineEdit, 2, 2, 1, 1);

        maxLineEdit = new QLineEdit(XlDOIDialogClass);
        maxLineEdit->setObjectName(QString::fromUtf8("maxLineEdit"));

        gridLayout->addWidget(maxLineEdit, 0, 2, 1, 1);

        DOIHistogram = new QGraphicsView(XlDOIDialogClass);
        DOIHistogram->setObjectName(QString::fromUtf8("DOIHistogram"));
        DOIHistogram->setEnabled(true);

        gridLayout->addWidget(DOIHistogram, 3, 0, 1, 4);

        okButton = new QPushButton(XlDOIDialogClass);
        okButton->setObjectName(QString::fromUtf8("okButton"));

        gridLayout->addWidget(okButton, 0, 3, 1, 1);


        retranslateUi(XlDOIDialogClass);

        QMetaObject::connectSlotsByName(XlDOIDialogClass);
    } // setupUi

    void retranslateUi(QDialog *XlDOIDialogClass)
    {
        XlDOIDialogClass->setWindowTitle(QApplication::translate("XlDOIDialogClass", "XlDOIDialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("XlDOIDialogClass", "Max:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("XlDOIDialogClass", "Min:", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("XlDOIDialogClass", "Cancel", 0, QApplication::UnicodeUTF8));
        okButton->setText(QApplication::translate("XlDOIDialogClass", "OK", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class XlDOIDialogClass: public Ui_XlDOIDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XLDOIDIALOG_H
