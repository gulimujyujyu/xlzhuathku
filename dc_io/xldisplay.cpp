#include "xldisplay.h"
#include <QMouseEvent>
#include <QStylePainter>

XlDisplay::XlDisplay(QWidget *parent)
	: QLabel(parent)
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	this->data = QImage(640,480,QImage::Format_RGB32);
	this->setPixmap(QPixmap::fromImage(data));
	this->isSelecting = false;
	this->doneSelection = false;
}

XlDisplay::~XlDisplay()
{

}

void XlDisplay::putImage(QImage img)
{
	this->data = img;
	this->setPixmap(QPixmap::fromImage(data));
}

QImage XlDisplay::getImage()
{
	return this->data;
}

void XlDisplay::mousePressEvent(QMouseEvent *ev)
{
	QRect rect(margin(), margin(), width() - 2*margin(), height() - 2*margin());

	if (ev->button() == Qt::LeftButton) {
		if(rect.contains(ev->pos())) {
			doneSelection = false;
			isSelecting = true;
			selectionRectangle.setTopLeft(ev->pos());
			selectionRectangle.setTopRight(ev->pos());
			updateSelection();
			setCursor(Qt::CrossCursor);
		}
	}
}


void XlDisplay::mouseMoveEvent(QMouseEvent *ev)
{
	if(isSelecting) {
		updateSelection();
		doneSelection = false;
		selectionRectangle.setBottomRight(ev->pos());
		updateSelection();
	}
}

void XlDisplay::mouseReleaseEvent(QMouseEvent *ev)
{
	if (isSelecting && (ev->button() == Qt::LeftButton)) {
		isSelecting = false;
		doneSelection = true;

		//check if it is valid
		this->selectionRectangle = this->selectionRectangle.normalized();
		if (selectionRectangle.width() < 20 || selectionRectangle.height() < 20) {
			doneSelection = false;
		}

		updateSelection();
		unsetCursor();
	}
}

void XlDisplay::drawSelectionPoints()
{
	QPainter painter(this);
	painter.setPen(palette().light().color());

	if (doneSelection) {
		painter.drawPoint(selectionRectangle.topLeft());
		painter.drawPoint(selectionRectangle.bottomRight());
	}
}

void XlDisplay::paintEvent(QPaintEvent *event)
{
	QStylePainter painter(this);
	const QPixmap *a = this->pixmap();
	painter.drawPixmap(0,0,*a);
	if(isSelecting) {
		painter.setPen(palette().light().color());
		painter.drawRect(selectionRectangle.normalized().adjusted(0,0,-1,-1));
	}
	if (doneSelection) {
		painter.drawPoint(selectionRectangle.topLeft());
		painter.drawPoint(selectionRectangle.bottomRight());
	}
}

void XlDisplay::updateSelection()
{
	QRect rect = selectionRectangle.normalized();
	update(rect.left(),rect.top(),rect.width(),1);
	update(rect.left(),rect.top(),1,rect.height());
	update(rect.left(),rect.bottom(),rect.width(),1);
	update(rect.right(),rect.bottom(),1,rect.height());
}

QRect XlDisplay::getSelectionRect()
{
	return this->selectionRectangle;
}

bool XlDisplay::hasDoneSelection()
{
	return this->doneSelection;
}