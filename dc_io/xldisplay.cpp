#include "xldisplay.h"

XlDisplay::XlDisplay(QWidget *parent)
	: QLabel(parent)
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	this->data = QImage(640,480,QImage::Format_RGB32);
	this->setPixmap(QPixmap::fromImage(data));
}

XlDisplay::~XlDisplay()
{

}

void XlDisplay::putImage(QImage img)
{
	this->data = img;
	this->setPixmap(QPixmap::fromImage(data));
}