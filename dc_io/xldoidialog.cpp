#include "xldoidialog.h"
#include <QRegExp>

XlDOIDialog::XlDOIDialog(QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);
	ui.okButton->setEnabled(false);

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui.maxLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(checkLineEdits()));
	connect(ui.minLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(checkLineEdits()));
}

XlDOIDialog::~XlDOIDialog()
{

}

int XlDOIDialog::getMinValue()
{
	return ui.minLineEdit->text().toInt();
}

int XlDOIDialog::getMaxValue()
{
	return ui.maxLineEdit->text().toInt();
}	

void XlDOIDialog::checkLineEdits()
{
	QRegExp re("[0-9]{1,4}");
	ui.maxLineEdit->setValidator(new QRegExpValidator(re, this));
	ui.minLineEdit->setValidator(new QRegExpValidator(re, this));

	if ( ui.maxLineEdit->hasAcceptableInput() && ui.minLineEdit->hasAcceptableInput()) {
		ui.okButton->setEnabled(true);
	}
}