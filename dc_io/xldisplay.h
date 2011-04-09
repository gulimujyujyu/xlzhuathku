#ifndef XLDISPLAY_H
#define XLDISPLAY_H

#include <QLabel>
#include <QStylePainter>

class XlDisplay : public QLabel
{
	Q_OBJECT

public:
	XlDisplay(QWidget *parent);
	~XlDisplay();

	void putImage(QImage img);
	QImage getImage();
	QRect getSelectionRect();
	bool hasDoneSelection();

protected:
	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void mouseReleaseEvent(QMouseEvent *ev);
	void paintEvent(QPaintEvent *event);

private:
	QImage data;
	bool isSelecting;
	bool doneSelection;
	QRect selectionRectangle;

	void drawSelectionPoints();
	void updateSelection();
};

#endif // XLDISPLAY_H
