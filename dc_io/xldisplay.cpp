#include "xldisplay.h"
#include <QImage>

/*
 *	Class:	XlDisplay
 */
XlDisplay::XlDisplay(QWidget *parent)
	: QWidget(parent)
{
	layout = new QVBoxLayout;
	imageLabel = new QLabel;
	layout->addWidget(imageLabel);

	setDisplaySettings(XlDisplaySettings());

	setBackgroundRole(QPalette::Dark);
	setAutoFillBackground(true);

	//TODO
	QImage dummy(640,480,QImage::Format_RGB32);
	pixmap = dummy;
	imageLabel->setPixmap(QPixmap::fromImage(pixmap));
	
	setLayout(layout);
}

XlDisplay::~XlDisplay()
{

}

void XlDisplay::putImage(QImage img)
{
	this->pixmap = img;
	imageLabel->setPixmap(QPixmap::fromImage(this->pixmap));
}

void XlDisplay::setDisplaySettings(const XlDisplaySettings & settings) 
{
	this->settings = settings;
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