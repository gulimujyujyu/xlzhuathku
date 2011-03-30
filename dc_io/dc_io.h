#ifndef DC_IO_H
#define DC_IO_H

#include <QtGui/QMainWindow>
#include "ui_dc_io.h"
//kinect
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

class dc_io : public QMainWindow
{
	Q_OBJECT

public:
	dc_io(QWidget *parent = 0, Qt::WFlags flags = 0);
	~dc_io();
	void refreshStatusBar(QString ctn);

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

private:
	//Kinect
	XnStatus rc;
	xn::Context g_Context;
	xn::DepthGenerator g_DepthGenerator;
	xn::ImageGenerator g_ImageGenerator;
	xn::SceneAnalyzer g_SceneAnalyzer;
	xn::Recorder* g_pRecorder;

	//Status
	bool playFlag;
	bool recordFlag;

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
