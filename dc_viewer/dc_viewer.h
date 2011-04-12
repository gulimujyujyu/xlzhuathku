#ifndef DC_VIEWER_H
#define DC_VIEWER_H

#include <QtGui/QMainWindow>
#include <QFileSystemModel>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "ui_dc_viewer.h"

class dc_viewer : public QMainWindow
{
	Q_OBJECT

public:
	dc_viewer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~dc_viewer();

private:
	Ui::dc_viewerClass ui;

	//Model
	QFileSystemModel *fileModel;

	//Cached Data
	IplImage *colorImage;
	IplImage *depthImage;
	int maxDepth;
	int minDepth;
};

#endif // DC_VIEWER_H
