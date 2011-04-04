#ifndef XLDISPLAY_H
#define XLDISPLAY_H

#include <QLabel>

class XlDisplay : public QLabel
{
	Q_OBJECT

public:
	XlDisplay(QWidget *parent);
	~XlDisplay();

	void putImage(QImage img);

private:
	QImage data;
};

#endif // XLDISPLAY_H
