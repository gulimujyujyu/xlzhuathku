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
	bool setROR();
	bool scale1();
	bool scale05();
	bool scale2();
	//button slots
	void play();
	void capture();
	void record();

protected:
	//overload functions
	void timerEvent(QTimerEvent *event);

private:
	//paint related functions
	void getData();
	void drawScene();
	void initializeDataAndTimer();
	void initKinectParam();

private:
	//Kinect
	XnStatus rc;
	xn::Context g_Context;
	xn::Recorder* g_pRecorder;

	xn::DepthGenerator g_DepthGenerator;
	xn::ImageGenerator g_ImageGenerator;

	xn::DepthMetaData g_DepthData;
	xn::ImageMetaData g_ImageData;

	//Status
	bool playFlag;
	bool recordFlag;
	int timerId;
	int viewType;

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
