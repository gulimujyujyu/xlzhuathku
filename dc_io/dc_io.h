#ifndef DC_IO_H
#define DC_IO_H

#include <QtGui/QMainWindow>
#include "ui_dc_io.h"
//kinect
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

#define MAINWINDOW_VIEWTYPE_RAW 0
#define MAINWINDOW_VIEWTYPE_REGISTERED 1
#define MAINWINDOW_VIEWTYPE_3D 2

#define KINECT_MAXDEPTH 10000
#define ACCEPTABLE_MAX_DEPTH 3000
#define ACCEPTABLE_MIN_DEPTH 1
#define ACCEPTABLE_DEPTH_DIFFERENCE 10

class dc_io : public QMainWindow
{
	Q_OBJECT

public:
	dc_io(QWidget *parent = 0, Qt::WFlags flags = 0);
	~dc_io();
	void refreshStatusBar(QString ctn);
	//Utilities Functions
	int inline mapDepthToIntensity( unsigned int nValue);

	static const int MinimalTimerInterval = 100;

private slots:
	bool connectCamera();
	bool changeViewRaw();
	bool changeViewRegistered();
	bool changeView3D();
	bool setROI();
	bool setDOI();
	bool scale1();
	bool scale05();
	bool scale2();
	//button slots
	void play();
	void capture();
	void captureROI();
	void record();
	//slider slots
	void changeMinValue();
	void changeMaxValue();

protected:
	//overload functions
	void timerEvent(QTimerEvent *event);
	void resizeEvent(QResizeEvent *event);
	void keyPressEvent(QKeyEvent *event);

private:
	//paint related functions
	void getData();
	void drawScene();
	void initializeDataAndTimer();
	void initKinectParam();
	bool checkROI(QRect& rct);

private:
	//Kinect
	XnStatus rc;
	xn::Context g_Context;
	xn::Recorder* g_pRecorder;

	xn::DepthGenerator g_DepthGenerator;
	xn::ImageGenerator g_ImageGenerator;

	xn::DepthMetaData g_DepthData;
	xn::ImageMetaData g_ImageData;

	XnDepthPixel g_MaxDepth;
	XnDepthPixel g_DepthHistogram;

	//Region of Interest
	int depthLowerBound;
	int depthUpperBound;
	QRect regionOfInterest;

	//Status
	bool hasROI;
	bool playFlag;
	bool recordFlag;
	int timerId;
	int viewType;
	double scaleFactor;

	//Display
	QImage *displayImage;
	Ui::dc_ioClass ui;
};

//Kinect related Macros
#define SAMPLE_XML_PATH "./Data/Sample-Scene.xml"
#define CHECK_RC(rc, what)											\
	if (rc != XN_STATUS_OK)											\
{																\
	printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
}

#endif // DC_IO_H
