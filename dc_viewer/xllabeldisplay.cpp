#include "xllabeldisplay.h"
#include <QDateTime>
#include <QAction>
#include <QMouseEvent>

XlLabelDisplay::XlLabelDisplay(QWidget *parent)
	: QLabel(parent)
{
	this->saveAction = new QAction("&Save", this);
	this->tempPath = "";
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
	img.save(this->tempPath + filename, "PNG");
}

/*
 *	Modify mouse press event
 */
void XlLabelDisplay::mouseDoubleClickEvent(QMouseEvent *ev)
{
	if(ev->button() == Qt::LeftButton)
		emit labelDoubleClicked();
}

/*
 *	set temp paths
 *	should contain '\', i.e. a = "aaaaa\\aaa\\a\\"
 */
void XlLabelDisplay::setTempPath(QString a)
{
	this->tempPath = a;
}