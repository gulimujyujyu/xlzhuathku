#ifndef DC_VIEWER_H
#define DC_VIEWER_H

#include <QtGui/QMainWindow>
#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "ui_dc_viewer.h"

class dc_viewer : public QMainWindow
{
	Q_OBJECT

public:
	dc_viewer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~dc_viewer();
	void refreshStatusBar(QString ctn);

	//Utilities functions
	QImage iplimage2qimage(IplImage *img);
	QImage calculateCurvature( IplImage *clrImg, IplImage *dptImg);
	QImage calculateEdge( IplImage *clrImg, IplImage *dptImg);
	QImage calculateNormal( IplImage *clrImg, IplImage *dptImg);
	QImage mapColor2Depth(IplImage *clrImg, IplImage *dptImg);

private slots:
	void setRootPath();
	void onFileItemDoubleClicked(QModelIndex idx);
	bool isValidROIImage(QFileInfo fileInfo, QString &prefix);
	
private:
	void loadFiles();
	void refreshAllLabels();

private:
	Ui::dc_viewerClass ui;

	//Model
	QFileSystemModel *fileModel;
	//QFileSystemWatcher *fileWatcher;
	QString rootPath;

	//Cached Data
	QString filePrefix;
	IplImage *colorImage;
	IplImage *depthImage;
	IplImage *captureImage;
	int maxDepth;
	int minDepth;
};

#endif // DC_VIEWER_H
