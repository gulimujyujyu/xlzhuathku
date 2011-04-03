#ifndef XLDISPLAY_H
#define XLDISPLAY_H

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QVBoxLayout>

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
	void putImage(QImage img);

private:
	QLabel *imageLabel;
	QVBoxLayout *layout;

	QImage pixmap;
	XlDisplaySettings settings;
};

#endif // XLDISPLAY_H
