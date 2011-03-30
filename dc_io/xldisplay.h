#ifndef XLDISPLAY_H
#define XLDISPLAY_H

#include <QWidget>
#include <QPixmap>

class XlDisplaySettings {
public:
	XlDisplaySettings();

	int height;
	int width;
	double zoomLevel;
};

class XlDisplay : public QWidget
{
	Q_OBJECT

public:
	XlDisplay(QWidget *parent);
	~XlDisplay();

	void setDisplaySettings(const XlDisplaySettings & settings);
	void setPixmap(QPixmap pm);
	void clearPixmap();
	void refreshPixmap();

protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);

private:
	QPixmap pixmap;
	XlDisplaySettings settings;
};

#endif // XLDISPLAY_H
