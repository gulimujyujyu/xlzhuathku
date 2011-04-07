#include "dc_io.h"
#include "xldoidialog.h"
#include <cmath>
#include <QtGui/QMessageBox>
#include <QKeyEvent>
#include <QPixmap>
#include <QTime>
#include <QFile>
#include <QTextStream>


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
	connect(ui.actionROI,SIGNAL(triggered()), this, SLOT(setROI()));
	connect(ui.actionDOI,SIGNAL(triggered()), this, SLOT(setDOI()));
	//connect buttons
	connect(ui.PlayButton, SIGNAL(clicked()), this, SLOT(play()));
	connect(ui.CaptureButton, SIGNAL(clicked()), this, SLOT(capture()));
	connect(ui.CaptureROIButton, SIGNAL(clicked()), this, SLOT(captureROI()));
	connect(ui.RecordButton, SIGNAL(clicked()), this, SLOT(record()));
	//connect sliders
	connect(ui.minSlider, SIGNAL(valueChanged(int)), this, SLOT(changeMinValue()));
	connect(ui.maxSlider, SIGNAL(valueChanged(int)), this, SLOT(changeMaxValue()));

	//initialize flags
	initializeDataAndTimer();
	//initialize Kinect
	initKinectParam();	
}

dc_io::~dc_io()
{

}

/*
 *	Initialize Kinect Related Parameters
 */
void dc_io::initKinectParam()
{
	//TODO:
	rc = XN_STATUS_OK + 999;
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

	rc = g_Context.StartGeneratingAll();
	CHECK_RC(rc, "StartGenerating");

	state=(rc==XN_STATUS_OK);
	if(!state) {
		QMessageBox::critical(this,"Connect Kinect Error", "state is not XN_STATUS_OK");
		this->refreshStatusBar(QString("Connect Kinect Error"));
		return state;
	}
	else {
		this->refreshStatusBar(QString("Connect Kinect Successful"));
	}

	//Display maximum distance - 10000
	g_MaxDepth = this->g_DepthGenerator.GetDeviceMaxDepth();
	this->refreshStatusBar(QString("Connect Kinect Successful. g_MaxDepth:") + QString::number(g_MaxDepth));

	//generate pixmap
	int imgWidth = this->g_ImageData.XRes();
	int imgHeight = this->g_ImageData.YRes();
	int dptWidth = this->g_DepthData.XRes();
	int dptHeight = this->g_DepthData.YRes();
	int dispHeight = (imgHeight>=dptHeight?imgHeight:dptHeight);
	int dispWidth = (imgWidth+dptWidth);
	this->regionOfInterest = QRect(0,0,dispWidth,dispHeight);

	//initialize data storage
	initializeDataAndTimer();
	return state;
}

/*
 *	Key Event: add keyboard short-cut
 */
void dc_io::keyPressEvent(QKeyEvent *event){
	switch (event->key())
	{
	case Qt::Key_C:
		this->capture();
		break;
	case Qt::Key_R:
		this->captureROI();
		break;
	default:
		break;
	}
}

/*
 *	Clear and New a data storage when connect new camera
 */
void dc_io::initializeDataAndTimer()
{
	if(this->timerId) {
		killTimer(this->timerId);
	}	
	this->timerId = startTimer(this->MinimalTimerInterval);
	this->hasROI = false;
	this->playFlag = false;
	this->recordFlag = false;
	this->timerId = 0;
	this->viewType = MAINWINDOW_VIEWTYPE_RAW;
	this->scaleFactor = 1;
	this->regionOfInterest = ui.Display->rect();

	this->depthUpperBound = 900;
	this->depthLowerBound = 700;
}

/*
 *	Scale image to twice the original size
 */
bool dc_io::scale2()
{
	bool state=true;
	this->scaleFactor = 2;

	return state;
}

/*
 *	Scale image to half size
 */
bool dc_io::scale05()
{
	bool state=true;
	this->scaleFactor = 0.5;

	return state;
}

/*
 *	Scale image to original size
 */
bool dc_io::scale1()
{
	bool state=true;
	this->scaleFactor = 1;

	return state;
}

/*
 *	Change View to 3D view
 */
bool dc_io::changeView3D()
{
	bool state=true;
	this->viewType = MAINWINDOW_VIEWTYPE_3D;
	if ( this->rc == XN_STATUS_OK) {
		//TODO: same as registered right now
		g_DepthGenerator.GetAlternativeViewPointCap().SetViewPoint( g_ImageGenerator); 		
	}

	this->refreshStatusBar(QString("View Point: 3D"));
	return state;
}

/*
 *	Change View to Registered data
 */
bool dc_io::changeViewRegistered()
{
	bool state=true;
	this->viewType = MAINWINDOW_VIEWTYPE_REGISTERED;
	if ( this->rc == XN_STATUS_OK) {
		//g_ImageGenerator.GetAlternativeViewPointCap().SetViewPoint(g_DepthGenerator);
		g_DepthGenerator.GetAlternativeViewPointCap().SetViewPoint( g_ImageGenerator); 		
	}

	this->refreshStatusBar(QString("View Point: REGISTERED"));
	return state;
}

/*
 *	Change View to Raw data
 */
bool dc_io::changeViewRaw()
{
	bool state=true;
	this->viewType = MAINWINDOW_VIEWTYPE_RAW;

	if ( this->rc == XN_STATUS_OK) {
		//g_ImageGenerator.GetAlternativeViewPointCap().ResetViewPoint();
		g_DepthGenerator.GetAlternativeViewPointCap().ResetViewPoint();
	}

	this->refreshStatusBar(QString("View Point: RAW"));
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
	if ( this->rc != XN_STATUS_OK) {
		return;
	}

	QString filename;

	filename = QDateTime::currentDateTime().toString("yyyy_dd_MM_hh_mm_ss_zzz")+QString("capture.png");

	if(ui.Display->getImage().save(filename,"PNG")) {
		this->refreshStatusBar(QString("Successfully saved to ")+filename);
	} else {
		this->refreshStatusBar(QString("Cannot capture data."));
	}

}

/*
 *	capture ROI of one frame and write
 */
void dc_io::captureROI()
{
	if (this->rc != XN_STATUS_OK)
	{
		return;
	}
	QString prefix;
	QString dfn;
	QString cfn;
	QString ifn;
	QString txtfn;

	prefix = QDateTime::currentDateTime().toString("yyyy_dd_MM_hh_mm_ss_zzz");
	dfn = prefix + QString("depth.png");
	cfn = prefix + QString("image.png");
	ifn = prefix + QString("capture.png");
	txtfn = prefix + QString("detail.box");

	int singleImageWidth = ui.Display->getImage().width()/2;

	QRect cRect(regionOfInterest.topLeft().x()%singleImageWidth,regionOfInterest.topLeft().y(),regionOfInterest.width(),regionOfInterest.height());
	QRect dRect(regionOfInterest.topLeft().x()%singleImageWidth+singleImageWidth,regionOfInterest.topLeft().y(),regionOfInterest.width(),regionOfInterest.height());
	
	QImage di = ui.Display->getImage().copy(dRect);
	QImage ci = ui.Display->getImage().copy(cRect);

	if(ui.Display->getImage().save(ifn,"PNG") && di.save(dfn,"PNG") && ci.save(cfn,"PNG")) {
		QFile file(txtfn);
		if( !file.open(QIODevice::WriteOnly)) {
			this->refreshStatusBar( QString("Cannot write to detail.txt but images saved"));
		}
		QTextStream out(&file);
		out << cRect.topLeft().x() << endl
			<< cRect.topLeft().y() << endl
			<< cRect.width() << endl
			<< cRect.height() << endl;
		file.close();
		this->refreshStatusBar(QString("Successfully saved to ")+prefix);
	} else {
		this->refreshStatusBar(QString("Cannot capture data."));
	}
}

/*
 *	Record frame and write into files
 */
void dc_io::record()
{
	//TODO: 
	bool state=true;
	this->recordFlag = ui.RecordButton->isChecked();
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
	bool state=true;

	if ( ui.Display->hasDoneSelection()) {
		if ( checkROI(ui.Display->getSelectionRect())){
			this->regionOfInterest = ui.Display->getSelectionRect();	
			this->refreshStatusBar(QString("Set ROI Done. TL=(")+
				QString::number(regionOfInterest.topLeft().x())+QString(",")+QString::number(regionOfInterest.topLeft().y())+
				QString("), BR=(")+
				QString::number(regionOfInterest.bottomRight().x())+QString(",")+QString::number(regionOfInterest.bottomRight().y())+
				QString(")"));
		}
		else {
			state = false;
			this->refreshStatusBar(QString("ROI should be in either color or depth image."));
		}		
	} 
	else {
		state = false;
		this->refreshStatusBar(QString("Please first select a retangle within the picture above."));
	}
	this->hasROI = state;
	return state;
}

/*
 *	Check whether ROI is within a valid area
 */
bool dc_io::checkROI(QRect &rct)
{
	QRect tmp = ui.Display->getImage().rect();
	QRect tmp1(0,0,tmp.width()/2,tmp.height());
	QRect tmp2(tmp.width()/2,0,tmp.width()/2,tmp.height());

	return (tmp1.contains(rct) || tmp2.contains(rct));
}

/*
 *	set Region of interested Depth
 */
bool dc_io::setDOI()
{
	bool state=true;

	XlDOIDialog doiDialog(this);

	if(doiDialog.exec()) {
		int maxValue = doiDialog.getMaxValue();
		int minValue = doiDialog.getMinValue();

		if ( maxValue <= ACCEPTABLE_MAX_DEPTH && minValue >= ACCEPTABLE_MIN_DEPTH && maxValue - minValue >= ACCEPTABLE_DEPTH_DIFFERENCE) {
			this->depthUpperBound = maxValue;
			this->depthLowerBound = minValue;
			this->refreshStatusBar(QString("DOI....Max:")+QString::number(maxValue) + QString(", \tMin: ") + QString::number(minValue));
		} 
		else {
			this->refreshStatusBar(QString("Failed to set DOI. Is it within [1,10000]?"));
		}		
	}

	return state;
}

/*
 *	Paint Function
 */
void dc_io::timerEvent(QTimerEvent *event)
{
	if (rc == XN_STATUS_OK && this->playFlag)	{
		QTime tmpTimer;
		tmpTimer.start();
		//TODO:
		getData(); // 3-7ms	
		drawScene(); // 42-46ms
		//this->refreshStatusBar(QString("One Frame: ") + QString::number(tmpTimer.elapsed()) + QString(" ms."));		
	}	
	this->resize(this->minimumSize());
}

/*
 *	get data from generators
 */
void dc_io::getData()
{
	if (this->playFlag) {
		g_Context.WaitAndUpdateAll();
		
		this->g_DepthGenerator.GetMetaData(this->g_DepthData);
		this->g_ImageGenerator.GetMetaData(this->g_ImageData);
	}	
}

/*
 *	Draw Scene in XlDisplay Widget from data
 */
void dc_io::drawScene()
{
	//generate pixmap
	int imgWidth = this->g_ImageData.XRes();
	int imgHeight = this->g_ImageData.YRes();
	int dptWidth = this->g_DepthData.XRes();
	int dptHeight = this->g_DepthData.YRes();
	int dispHeight = (imgHeight>=dptHeight?imgHeight:dptHeight);
	int dispWidth = (imgWidth+dptWidth);
	//int dispHeight = dptHeight;
	//int dispWidth = dptWidth;

	QImage disp(dispWidth,dispHeight,QImage::Format_RGB32);
	QColor pxl(0,0,0);
	int i,j;
	int tmp, tr, tg, tb;

	const XnRGB24Pixel* pImage = this->g_ImageData.RGB24Data();
	const XnDepthPixel* pDepth = this->g_DepthData.Data();
	unsigned int nValue;

	//set one frame

	for ( i = 0; i < dispHeight; i++) {
		for (j=0; j< imgWidth; j++) {
			tr = pImage->nRed;
			tg = pImage->nGreen;
			tb = pImage->nBlue;
			disp.setPixel(j,i,qRgb(tr, tg, tb));
			pImage ++;
		}
		for (j=0; j<dptWidth; j++) {
			nValue = *pDepth;
			pDepth++;
			tmp = mapDepthToIntensity( nValue);
			disp.setPixel(j+imgWidth,i,qRgb(tmp, tmp, tmp));
		}
	}
	
	//scale
	if (fabs(scaleFactor-1) >= 1e-7) {
		QImage scaledDisp = disp.scaled(int (dispWidth*scaleFactor), int (dispHeight*scaleFactor));
		ui.Display->putImage(scaledDisp);	
	} 
	else {
		ui.Display->putImage(disp);	
	}	
}

/*
 *	Force resize
 */

void dc_io::resizeEvent(QResizeEvent *event)
{
	this->ui.gridLayout->activate();
}
/*
 *	Map Depth Value to a [0,255] intensity value
 */
int inline dc_io::mapDepthToIntensity( unsigned int nValue)
{
	int tmp = 0;
	//linear scale to [0,255]
	const int a = this->depthUpperBound;
	const int b = this->depthLowerBound;
	if (nValue != 0 && nValue < a && nValue > b)
		tmp = int(255*(a-(float)nValue)/float(a-b));
	tmp = tmp>255? 0:tmp;
	tmp = tmp<0? 0:tmp;

	return tmp;
}

/*
 *	If MinSlider value changed, change Min Value
 */
void dc_io::changeMinValue()
{
	int a = ui.minSlider->value();
	float unit = float(this->depthUpperBound - ACCEPTABLE_MIN_DEPTH)/100.0;
	this->depthLowerBound = ACCEPTABLE_MIN_DEPTH + int(a*unit);
	
	this->refreshStatusBar(QString("DOI....Max:")+QString::number(depthUpperBound) + QString(", \tMin: ") + QString::number(depthLowerBound));
}

/*
 *	If MaxSlider value changed, change Max Value
 */
void dc_io::changeMaxValue()
{
	int a = ui.maxSlider->value();
	float unit = float(ACCEPTABLE_MAX_DEPTH - this->depthLowerBound)/100.0;
	this->depthUpperBound = this->depthLowerBound + int(a*unit);

	this->refreshStatusBar(QString("DOI....Max:")+QString::number(depthUpperBound) + QString(", \tMin: ") + QString::number(depthLowerBound));
}