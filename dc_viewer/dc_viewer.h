#ifndef DC_VIEWER_H
#define DC_VIEWER_H

#include <QtGui/QMainWindow>
#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "ui_dc_viewer.h"

#define DEPTH_LOOKUP_TABLE_SIZE 256
#define COLOR_LOOKUP_TABLE_SIZE 256

#define NORMAL_NEIGHBORHOOD_SIZE 9
#define NORMAL_NEIGHBORHOOD_SIZE_SQUARE 81 // = NORMAL_NEIGHBORHOOD_SIZE*NORMAL_NEIGHBORHOOD_SIZE
#define NORMAL_NEIGHBORHOOD_SIZE_SQUARE_BY_3 273 // = NORMAL_NEIGHBORHOOD_SIZE*NORMAL_NEIGHBORHOOD_SIZE*3
#define NORMAL_NEIGHBORHOOD_SIZE_HALF 4 // = NORMAL_NEIGHBORHOOD_SIZE/2
#define NORMNUM_THRESHOLD 40 // = NORMAL_NEIGHBORHOOD_SIZE_SQUARE/2

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
	inline bool isInBoundingBox(int x, int y);
	inline void normalColorMapping(float &dr, float &dg, float &db, float &r, float &g, float &b);

private slots:
	void setRootPath();
	void setTemppath();
	void onFileItemDoubleClicked(QModelIndex idx);
	bool isValidROIImage(QFileInfo fileInfo, QString &prefix);
	
private:
	void loadFiles();
	void initializeColorMap();
	void loadPropertyFile();
	void calculate3DData();
	void refreshAllLabels();
	void setDepthLookupTable();

private:
	Ui::dc_viewerClass ui;

	//Model
	QFileSystemModel *fileModel;
	//QFileSystemWatcher *fileWatcher;
	QString rootPath;
	QString tempPath;
	double focusLength;
	double pixelSize;

	//Cached Data
	double depthLU[DEPTH_LOOKUP_TABLE_SIZE];
	int colorMapR[COLOR_LOOKUP_TABLE_SIZE];
	int colorMapB[COLOR_LOOKUP_TABLE_SIZE];
	int colorMapG[COLOR_LOOKUP_TABLE_SIZE];
	QPoint boundingBox;
	QString filePrefix;
	IplImage *colorImage;
	IplImage *depthImage;
	IplImage *captureImage;
	CvMat *normalData;
	CvMat *pointData;
	int depthHeight;
	int depthWidth;
	int maxDepth;
	int minDepth;
};

#endif // DC_VIEWER_H
