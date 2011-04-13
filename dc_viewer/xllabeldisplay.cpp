#include "xllabeldisplay.h"
#include <QDateTime>
#include <QAction>

XlLabelDisplay::XlLabelDisplay(QWidget *parent)
	: QLabel(parent)
{
	this->saveAction = new QAction("&Save", this);
	connect(this->saveAction, SIGNAL(triggered()), this, SLOT(saveImageLocally()));
	createContextMenu();
}

XlLabelDisplay::~XlLabelDisplay()
{

}

/*
 *	Create Context Menu
 */
void XlLabelDisplay::createContextMenu()
{
	this->addAction(saveAction);
	this->setContextMenuPolicy(Qt::ActionsContextMenu);
}

/*
 *	Save image locally
 */
void XlLabelDisplay::saveImageLocally()
{
	QImage img = this->pixmap()->toImage();
	QString filename = QDateTime::currentDateTime().toString("yyyy_dd_MM_hh_mm_ss_zzz")+QString(".png");
	img.save(filename, "PNG");
}