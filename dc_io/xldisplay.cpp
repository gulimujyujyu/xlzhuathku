#include "xldisplay.h"
#include <QStylePainter>

/*
 *	Class:	XlDisplay
 */
XlDisplay::XlDisplay(QWidget *parent)
	: QWidget(parent)
{
	setBackgroundRole(QPalette::Dark);
	setAutoFillBackground(true);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setFocusPolicy(Qt::StrongFocus);
	setDisplaySettings(XlDisplaySettings());
}

XlDisplay::~XlDisplay()
{

}

void XlDisplay::paintEvent(QPaintEvent *event)
{
	QStylePainter painter(this);
	painter.drawPixmap(0,0,this->pixmap);
}

void XlDisplay::resizeEvent(QResizeEvent *event)
{

}

void XlDisplay::setPixmap(QPixmap pm)
{
	this->pixmap = pm;
	refreshPixmap();
}

void XlDisplay::setDisplaySettings(const XlDisplaySettings & settings) 
{
	this->settings = settings;
}

void XlDisplay::refreshPixmap()
{
	this->pixmap = QPixmap(this->size());
	this->pixmap.fill(this,0,0);

	QPainter painters(&pixmap);
	painters.initFrom(this);
	update();
}
/*
 *	Class:	XlDisplaySettings 
 */
XlDisplaySettings::XlDisplaySettings()
{
	this->zoomLevel = 1;
	this->height = 0;
	this->width = 1;
}