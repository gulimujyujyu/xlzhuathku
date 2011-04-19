#include "dc_viewer.h"
#include <QtGui/QFileDialog>
#include <QFile>
#include <QTextStream>

dc_viewer::dc_viewer(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	//connect signals
	connect(this->ui.actionSetRoot, SIGNAL(triggered()), this, SLOT(setRootPath()));
	connect(this->ui.actionSetTemppath, SIGNAL(triggered()), this, SLOT(setTemppath()));
	connect(this->ui.fileTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onFileItemDoubleClicked(QModelIndex)));

	//set parameters
	this->colorImage = NULL;
	this->depthImage = NULL;
	this->captureImage = NULL;
	this->pointData = NULL;
	this->normalData = NULL;
	this->fileModel = new QFileSystemModel(this);
	this->rootPath = QDir::currentPath();
	this->fileModel->setRootPath(this->rootPath);
	this->boundingBox.setX(0);
	this->boundingBox.setY(0);
	this->focusLength = 1;
	this->pixelSize = 1;
	this->initializeColorMap();
	//QStringList roiNameFilter;
	//roiNameFilter << "png";
	//this->fileModel->setNameFilters(roiNameFilter);
	this->ui.fileTreeView->setModel(fileModel);
	this->ui.fileTreeView->setCurrentIndex(fileModel->index(this->rootPath));
}

dc_viewer::~dc_viewer()
{

}

/*
 *	Initialize Color Map for normal visualization
 */
void dc_viewer::initializeColorMap()
{
	for (int i=0; i<256; i++)
	{
		colorMapR[i] = 0+i;
		colorMapG[i] = abs((255-2*i));
		colorMapB[i] = 255-i;
	}
}

/*
 *	Set temppath for labels to save locally.
 */
void dc_viewer::setTemppath()
{
	this->tempPath = QFileDialog::getExistingDirectory(this, "Set tempPath Dir", QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	this->tempPath = tempPath + QString("\\");
	ui.depthLabel->setTempPath(tempPath);
	ui.colorLabel->setTempPath(tempPath);
	ui.normalLabel->setTempPath(tempPath);
	ui.curvatureLabel->setTempPath(tempPath);
	ui.tempLabel->setTempPath(tempPath);
	ui.edgeLabel->setTempPath(tempPath);
}

/*
 *	Set Root Path for TreeView
 */
void dc_viewer::setRootPath()
{
	this->rootPath = QFileDialog::getExistingDirectory(this, "Set Root Dir", QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	this->loadPropertyFile();
	this->ui.fileTreeView->collapseAll();
	this->ui.fileTreeView->setCurrentIndex(fileModel->index(this->rootPath));
	this->ui.fileTreeView->expand(fileModel->index(this->rootPath));
}

/*
 *	Load Property Files
 */
void dc_viewer::loadPropertyFile()
{
	QString filename = this->rootPath + QString("\\kinect.property");	
	QFile file(filename);

	if(!file.open(QIODevice::ReadOnly)) {
		this->refreshStatusBar("Load kinect.property failed.");
	}
	QTextStream fin(&file);
	fin >> this->focusLength >> this->pixelSize;
	file.close();
}

/*
 *	If a file is double-clicked,
 *	process it if it is an ROI image.
 */
void dc_viewer::onFileItemDoubleClicked(QModelIndex idx)
{
	QFileInfo fileInfo = this->fileModel->fileInfo(idx);

	QString prefix;
	if ( this->isValidROIImage(fileInfo, prefix)) {
		QFileInfo fi;
		fi.setFile(fileInfo.path(), prefix);
		this->filePrefix = fi.filePath();
		loadFiles();
		setDepthLookupTable();
		calculate3DData();
		refreshAllLabels();
	}	
}

/*
 *	Load Files by prefix
 */
void dc_viewer::loadFiles()
{
	if (this->colorImage) cvReleaseImage(&(this->colorImage));
	if (this->depthImage) cvReleaseImage(&(this->depthImage));
	if (this->captureImage) cvReleaseImage(&(this->captureImage));
	if (this->normalData) cvReleaseMat(&(this->normalData));
	if (this->pointData) cvReleaseMat(&(this->pointData));

	QString re_color(filePrefix + QString("image.png"));
	QString re_depth(filePrefix + QString("depth.png"));
	QString re_box(filePrefix + QString("detail.box"));

	//QString re_capture(filePrefix + QString("capture.png"));
	QFile file(re_box);
	int tmp, tmpX, tmpY;
	if(!file.open(QIODevice::ReadOnly)) {
		this->refreshStatusBar("Load bounding box failed.");
		return;
	}
	QTextStream fin(&file);
	fin >> tmp >> tmpX >> tmpY >> tmp >> tmp >> tmp >> tmp;
	file.close();
	this->boundingBox.setX(tmpX);
	this->boundingBox.setY(tmpY);

	this->colorImage = cvLoadImage(re_color.toLatin1().data());
	this->depthImage = cvLoadImage(re_depth.toLatin1().data());

	this->depthWidth = colorImage->width;
	this->depthHeight = colorImage->height;

	this->pointData = cvCreateMat(depthImage->height*depthImage->width, 3, CV_32FC1);
	this->normalData = cvCreateMat(depthImage->height*depthImage->width, 3, CV_32FC1);
	cvZero(normalData);
	cvZero(pointData);
}

/*
 *	Calculate 3D information
 */
void dc_viewer::calculate3DData()
{
	//TODO: points
	//X = (u - 320) * depth_md_[k] * pixel_size_ * 0.001 / F_; 
	//Y = (v - 240) * depth_md_[k] * pixel_size_ * 0.001 / F_; 
	//Z = depth_md_[k] * 0.001; // from mm in meters! 
	int depthHeight = this->depthImage->height;
	int depthWidth = this->depthImage->width;
	int y,x;
	int offsetX = this->boundingBox.x();
	int offsetY = this->boundingBox.y();
	double pxlSize = pixelSize;
	double fcsLength = focusLength;
	double X,Y,Z,XYZ;
	char *data = this->depthImage->imageData;
	uchar tmpDepth = 0;
	int channels = this->depthImage->nChannels;

	for ( y=0; y<depthHeight; y++, data += depthImage->widthStep) {
		for( x=0; x<depthWidth; x++) {
			tmpDepth = uchar(data[x*channels]);
			X = double(x+offsetX - 320) * depthLU[tmpDepth] * pxlSize * 0.001 / fcsLength; 
			Y = double(y+offsetY - 240) * depthLU[tmpDepth] * pxlSize * 0.001 / fcsLength; 
			Z = depthLU[tmpDepth] * 0.001; // from mm in meters! 
			*((float*) CV_MAT_ELEM_PTR(*(this->pointData),x+y*depthWidth,0)) = X;
			*((float*) CV_MAT_ELEM_PTR(*(this->pointData),x+y*depthWidth,1)) = Y;
			*((float*) CV_MAT_ELEM_PTR(*(this->pointData),x+y*depthWidth,2)) = Z;
		}
	}
	//TODO: normal
	float buffer[NORMAL_NEIGHBORHOOD_SIZE_SQUARE_BY_3] = {0};
	float bufferZ[NORMAL_NEIGHBORHOOD_SIZE_SQUARE] = {0};
	CvMat *tmpMatLeft;
	CvMat *tmpMatRight;
	CvMat *tmpResult;
	int normNum;
	data = this->depthImage->imageData;
	for ( y=0; y<depthHeight; y++, data += depthImage->widthStep) {
		for( x=0; x<depthWidth; x++) {
			tmpDepth = uchar(data[x*channels]);
			normNum = 0;
			if (tmpDepth) {
				//get neighborhood	
				for ( int ii=-NORMAL_NEIGHBORHOOD_SIZE_HALF; ii<NORMAL_NEIGHBORHOOD_SIZE_HALF+1; ii++) {
					for ( int jj=-NORMAL_NEIGHBORHOOD_SIZE_HALF; jj<NORMAL_NEIGHBORHOOD_SIZE_HALF+1; jj++) {
						if (isInBoundingBox(x+ii,y+jj)) {
							tmpDepth = uchar(data[(x+ii)*channels+jj*depthImage->widthStep]);
							if (tmpDepth) {
								buffer[normNum*3] = CV_MAT_ELEM(*(this->pointData),float,(x+ii)+(y+jj)*depthWidth,0);
								buffer[normNum*3+1] = CV_MAT_ELEM(*(this->pointData),float,(x+ii)+(y+jj)*depthWidth,1);
								buffer[normNum*3+2] = 1;
								bufferZ[normNum] = CV_MAT_ELEM(*(this->pointData),float,(x+ii)+(y+jj)*depthWidth,2);
								normNum++;
							}
						}
					}
				}
				//calculate normal
				if (normNum >= NORMNUM_THRESHOLD) {
					tmpMatLeft = cvCreateMat(normNum,3,CV_32FC1);
					tmpResult = cvCreateMat(3,1,CV_32FC1);
					tmpMatRight = cvCreateMat(normNum,1,CV_32FC1);
					cvSetData(tmpMatLeft, buffer, tmpMatLeft->step);
					cvSetData(tmpMatRight, bufferZ, tmpMatRight->step);
					cvSolve(tmpMatLeft, tmpMatRight, tmpResult, CV_SVD);
					X = CV_MAT_ELEM(*(tmpResult),float,0,0);
					Y = CV_MAT_ELEM(*(tmpResult),float,1,0);
					Z = -1;
					XYZ = sqrt(X*X+Y*Y+Z*Z);
					X /= XYZ;
					Y /= XYZ;
					Z /= XYZ;
					*((float*) CV_MAT_ELEM_PTR(*(this->normalData),x+y*depthWidth,0)) = X;
					*((float*) CV_MAT_ELEM_PTR(*(this->normalData),x+y*depthWidth,1)) = Y;
					*((float*) CV_MAT_ELEM_PTR(*(this->normalData),x+y*depthWidth,2)) = Z;
					cvReleaseMat(&tmpMatLeft);
					cvReleaseMat(&tmpResult);
					cvReleaseMat(&tmpMatRight);
				}
				//calculate residue
			}			
		}
	}
}

inline bool dc_viewer::isInBoundingBox(int x, int y)
{
	return (x>=0 && x< this->depthWidth && y>=0 && y<this->depthHeight);
}

void dc_viewer::setDepthLookupTable()
{
	int depthItv = maxDepth-minDepth;
	double unit = double(depthItv)/255.0;
	for (int i = 0; i < DEPTH_LOOKUP_TABLE_SIZE; i++) {
		this->depthLU[i] = minDepth+double(i)*unit;
	}
	depthLU[0] = 0;
}

/*
 *	Refresh all the labels
 */
void dc_viewer::refreshAllLabels()
{
	//TODO
	//1. color
	QImage colorData = this->iplimage2qimage(this->colorImage);
	//2. depth
	QImage depthData = this->iplimage2qimage(this->depthImage);
	//3. normal
	QImage normalData = this->calculateNormal(colorImage,depthImage);
	//5. edge
	QImage edgeData = this->calculateEdge(colorImage,depthImage);
	//6. color depth
	QImage ColoredDepthData = this->mapColor2Depth(colorImage,depthImage);
	
	ui.colorLabel->setPixmap(QPixmap::fromImage(colorData));
	ui.depthLabel->setPixmap(QPixmap::fromImage(depthData));
	ui.normalLabel->setPixmap(QPixmap::fromImage(normalData));
	ui.edgeLabel->setPixmap(QPixmap::fromImage(edgeData));
	ui.tempLabel->setPixmap(QPixmap::fromImage(ColoredDepthData));
}

/*
 *	Calculate Normal from color image and depth image
 */
QImage dc_viewer::calculateNormal( IplImage *clrImg, IplImage *dptImg)
{
	int heightDepth = dptImg->height;
	int widthDepth = dptImg->width;
	int x,y;
	float nr,ng,nb;
	float rr,gg,bb;

	QImage qimg(dptImg->width, dptImg->height, QImage::Format_ARGB32);

	for( y = 0; y < heightDepth; y++) {
		for( x=0; x<widthDepth; x++) {
			nr = CV_MAT_ELEM(*(this->normalData),float,x+y*depthWidth,0);
			ng = CV_MAT_ELEM(*(this->normalData),float,x+y*depthWidth,1);
			nb = CV_MAT_ELEM(*(this->normalData),float,x+y*depthWidth,2);
			normalColorMapping(rr,gg,bb,nr,ng,nb);
			qimg.setPixel(x,y,qRgb(rr,gg,bb));
		}
	}

	return qimg;
}

/*
 *	Normal Color Mapping
 */
inline void dc_viewer::normalColorMapping(float &dr, float &dg, float &db, float &r, float &g, float &b)
{
	dr = colorMapR[int((r+1)*122.5)];
	dg = colorMapG[int((g+1)*122.5)];
	db = colorMapB[int((b+1)*122.5)];
}

/*
 *	Calculate Edge from color image and depth image
 */
QImage dc_viewer::calculateEdge(IplImage *clrImg, IplImage *dptImg)
{
	IplImage *edgeImage = cvCreateImage(cvSize(clrImg->width,clrImg->height), IPL_DEPTH_16S, 1);
	IplImage *grayImage = cvCreateImage(cvSize(clrImg->width,clrImg->height), IPL_DEPTH_8U, 1);
	cvCvtColor(clrImg, grayImage, CV_BGR2GRAY);
	//cvCopyImage(clrImg,edgeImage);
	cvSmooth(grayImage,grayImage,CV_GAUSSIAN,3,3,0,0);
	cvLaplace(grayImage,edgeImage,5);
	cvConvertScaleAbs(edgeImage , grayImage, 1 , 0);

	QImage qimg = this->iplimage2qimage(grayImage);
	cvReleaseImage(&edgeImage);
	cvReleaseImage(&grayImage);
	return qimg;
}

/*
 *	Calculate Curvature from color image and depth image
 */
QImage dc_viewer::calculateCurvature(IplImage *clrImg, IplImage *dptImg)
{
	//TODO
	QImage qimg;
	return qimg;
}

/*
 *	Calculate Curvature from color image and depth image
 */
QImage dc_viewer::mapColor2Depth(IplImage *clrImg, IplImage *dptImg)
{
	int h = dptImg->height;
	int w = dptImg->width;
	int channels = dptImg->nChannels;
	QImage qimg(w, h, QImage::Format_ARGB32);
	char *data = dptImg->imageData;
	char *colorData = clrImg->imageData;

	for (int y = 0; y < h; y++, data += dptImg->widthStep, colorData += clrImg->widthStep)
	{
		for (int x = 0; x < w; x++)
		{
			char r, g, b, a = 0;
			if (channels == 1)
			{
				r = data[x * channels]? colorData[x * channels]: 0;
				g = data[x * channels]? colorData[x * channels]: 0;
				b = data[x * channels]? colorData[x * channels]: 0;
			}
			else if (channels == 3 || channels == 4)
			{
				r = data[x * channels + 2]? colorData[x * channels+2]: 0;
				g = data[x * channels + 1]? colorData[x * channels+1]: 0;
				b = data[x * channels]? colorData[x * channels]: 0;
			}			

			if (channels == 4)
			{
				a = data[x * channels + 3];
				qimg.setPixel(x, y, qRgba(r, g, b, a));
			}
			else
			{
				qimg.setPixel(x, y, qRgb(r, g, b));
			}
		}
	}
	return qimg;
}

/*
 *	Check whether it is an ROI image
 */
bool dc_viewer::isValidROIImage(QFileInfo fileInfo, QString &prefix)
{
	//2011_12_04_13_58_45_513_931_752depth.png
	QRegExp reg_prefix("^(\\d{4}_\\d{2}_\\d{2}_\\d{2}_\\d{2}_\\d{2}_\\d{3}_\\d{1,5}_\\d{1,5})");
	QRegExp reg_depth("^\\d{4}_\\d{2}_\\d{2}_\\d{2}_\\d{2}_\\d{2}_\\d{3}_(\\d{1,5})_(\\d{1,5})");
	bool state = false;

	int pos = reg_prefix.indexIn(fileInfo.fileName());
	if (pos > -1) {
		prefix = reg_prefix.cap(1);
		QString re_color(prefix + QString("image.png"));
		QString re_depth(prefix + QString("depth.png"));
		QString re_capture(prefix + QString("capture.png"));
		QString re_box(prefix + QString("detail.box"));

		QFileInfo tmpFile;
		tmpFile.setFile(fileInfo.path(), re_color);
		if ( !(tmpFile.exists())){
			return state;
		}
		tmpFile.setFile(fileInfo.path(), re_depth);
		if ( !(tmpFile.exists())){
			return state;
		}
		tmpFile.setFile(fileInfo.path(), re_capture);
		if ( !(tmpFile.exists())){
			return state;
		}
		tmpFile.setFile(fileInfo.path(), re_box);
		if ( !(tmpFile.exists())){
			return state;
		}
		pos = reg_depth.indexIn(fileInfo.fileName());
		this->maxDepth = reg_depth.cap(1).toInt();
		this->minDepth = reg_depth.cap(2).toInt();
		state = true;
	}
	return state;
}

/*
 *	Refresh Status Bar
 */
void dc_viewer::refreshStatusBar(QString ctn)
{
	ui.statusBar->showMessage(ctn);
}

/*
 *	Convert IplImage Format to QImage Format
 */
QImage dc_viewer::iplimage2qimage(IplImage *iplImg)
{
	int h = iplImg->height;
	int w = iplImg->width;
	int channels = iplImg->nChannels;
	QImage qimg(w, h, QImage::Format_ARGB32);
	char *data = iplImg->imageData;

	for (int y = 0; y < h; y++, data += iplImg->widthStep)
	{
		for (int x = 0; x < w; x++)
		{
			char r, g, b, a = 0;
			if (channels == 1)
			{
				r = data[x * channels];
				g = data[x * channels];
				b = data[x * channels];
			}
			else if (channels == 3 || channels == 4)
			{
				r = data[x * channels + 2];
				g = data[x * channels + 1];
				b = data[x * channels];
			}

			if (channels == 4)
			{
				a = data[x * channels + 3];
				qimg.setPixel(x, y, qRgba(r, g, b, a));
			}
			else
			{
				qimg.setPixel(x, y, qRgb(r, g, b));
			}
		}
	}
	return qimg;
}