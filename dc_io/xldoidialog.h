#ifndef XLDOIDIALOG_H
#define XLDOIDIALOG_H

#include <QDialog>
#include "ui_xldoidialog.h"

class XlDOIDialog : public QDialog
{
	Q_OBJECT

public:
	XlDOIDialog(QWidget *parent = 0);
	~XlDOIDialog();

	int getMinValue();
	int getMaxValue();

private slots:
	void checkLineEdits();

private:
	Ui::XlDOIDialogClass ui;
};

#endif // XLDOIDIALOG_H
