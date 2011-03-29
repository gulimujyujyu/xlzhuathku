#include "dc_io.h"
#include <QtGui/QMessageBox>


dc_io::dc_io(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	//connect menu actions
	connect(ui.actionConnect, SIGNAL(triggered()), this, SLOT(connectCamera()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionRaw, SIGNAL(triggered()), this, SLOT(changeViewRaw()));
	connect(ui.actionRegistered, SIGNAL(triggered()), this, SLOT(changeViewRegistered()));
	connect(ui.action3D, SIGNAL(triggered()), this, SLOT(changeView3D()));
	connect(ui.action1, SIGNAL(triggered()), this, SLOT(scale1()));
	connect(ui.action05, SIGNAL(triggered()), this, SLOT(scale05()));
	connect(ui.action2, SIGNAL(triggered()), this, SLOT(scale2()));
	//connect buttons
	connect(ui.PlayButton, SIGNAL(clicked()), this, SLOT(play()));
	connect(ui.CaptureButton, SIGNAL(clicked()), this, SLOT(capture()));
	connect(ui.RecordButton, SIGNAL(clicked()), this, SLOT(record()));

	//initialize flags
	this->playFlag = false;
	this->recordFlag = false;

	//begin main loop
	startTimer(100);
	
}

dc_io::~dc_io()
{

}

/*
 *	Refresh Status Bar
 */
void dc_io::refreshStatusBar(QString ctn)
{
	ui.statusBar->showMessage(ctn);
}

/*
 *	Connect to camera
 */
bool dc_io::connectCamera()
{
	rc = XN_STATUS_OK;
	bool state=true;

	rc = g_Context.InitFromXmlFile(SAMPLE_XML_PATH);
	CHECK_RC(rc, "InitFromXml");

	rc = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	CHECK_RC(rc, "Find depth generator");
	rc = g_Context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_ImageGenerator);
	CHECK_RC(rc, "Find image generator");
	rc = g_Context.FindExistingNode(XN_NODE_TYPE_SCENE, g_SceneAnalyzer);
	CHECK_RC(rc, "Find scene analyzer");

	rc = g_Context.StartGeneratingAll();
	CHECK_RC(rc, "StartGenerating");

	state=(rc==XN_STATUS_OK);
	if(!state) {
		QMessageBox::critical(this,"Connect Kinect Error", "state is not XN_STATUS_OK");
	}
	return state;
}

/*
 *	Scale image to twice the original size
 */
bool dc_io::scale2()
{
	//TODO: 
	bool state=true;

	return state;
}

/*
 *	Scale image to half size
 */
bool dc_io::scale05()
{
	//TODO: 
	bool state=true;

	return state;
}

/*
 *	Scale image to original size
 */
bool dc_io::scale1()
{
	//TODO: 
	bool state=true;

	return state;
}

/*
 *	Change View to 3D view
 */
bool dc_io::changeView3D()
{
	//TODO: 
	bool state=true;

	return state;
}

/*
 *	Change View to Registered data
 */
bool dc_io::changeViewRegistered()
{
	//TODO: 
	bool state=true;

	return state;
}

/*
 *	Change View to Raw data
 */
bool dc_io::changeViewRaw()
{
	//TODO: 
	bool state=true;

	return state;
}

/*
 *	play
 */
void dc_io::play()
{
	playFlag = ui.PlayButton->isChecked();
	if (playFlag) {
		ui.PlayButton->setText("Pause");
	} else {
		ui.PlayButton->setText("Play");
	}
	this->refreshStatusBar(QString("PlayFlag=") + QString((playFlag?"true":"false")));
}

/*
 *	capture one frame and write
 */
void dc_io::capture()
{
	//TODO: 
	bool state=true;
}

/*
 *	Record frame and write into files
 */
void dc_io::record()
{
	//TODO: 
	bool state=true;
	this->recordFlag = ui.CaptureButton->isChecked();
	if (recordFlag) {
		ui.RecordButton->setText("Stop");
	} else {
		ui.RecordButton->setText("Record");
	}
	this->refreshStatusBar(QString("RecordFlag=") + QString((playFlag?"true":"false")));
}

/*
 *	set Region of interest
 */
bool dc_io::setROI()
{
	//TODO: 
	bool state=true;

	return state;
}

/*
 *	set Region of interested Depth
 */
bool dc_io::setROR()
{
	//TODO: 
	bool state=true;

	return state;
}

/*
 *	Piant Function
 */
void dc_io::timerEvent(QTimerEvent *event)
{
	if (rc != XN_STATUS_OK)	{
		//TODO:
	}
}