#include "dc_viewer.h"
#include <QtGui/QFileDialog>

dc_viewer::dc_viewer(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	//connect signals
	connect(this->ui.actionSetRoot, SIGNAL(triggered()), this, SLOT(setRootPath()));
	connect(this->ui.fileTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onFileItemDoubleClicked(QModelIndex)));

	//set parameters
	this->colorImage = NULL;
	this->depthImage = NULL;
	this->captureImage = NULL;
	this->fileModel = new QFileSystemModel(this);
	this->rootPath = QDir::currentPath();
	this->fileModel->setRootPath(this->rootPath);
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
 *	Set Root Path for TreeView
 */
void dc_viewer::setRootPath()
{
	this->rootPath = QFileDialog::getExistingDirectory(this, "Set Root Dir", QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	this->ui.fileTreeView->collapseAll();
	this->ui.fileTreeView->setCurrentIndex(fileModel->index(this->rootPath));
	this->ui.fileTreeView->expand(fileModel->index(this->rootPath));
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

	QString re_color(filePrefix + QString("image.png"));
	QString re_depth(filePrefix + QString("depth.png"));

	//QString re_capture(filePrefix + QString("capture.png"));
	//QString re_box(filePrefix + QString("detail.box"));

	this->colorImage = cvLoadImage(re_color.toLatin1().data());
	this->depthImage = cvLoadImage(re_depth.toLatin1().data());
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
	//5. edge
	QImage edgeData = this->calculateEdge(colorImage,depthImage);
	//6. color depth
	QImage ColoredDepthData = this->mapColor2Depth(colorImage,depthImage);
	
	ui.colorLabel->setPixmap(QPixmap::fromImage(colorData));
	ui.depthLabel->setPixmap(QPixmap::fromImage(depthData));
	ui.edgeLabel->setPixmap(QPixmap::fromImage(edgeData));
	ui.tempLabel->setPixmap(QPixmap::fromImage(ColoredDepthData));
}

/*
 *	Calculate Normal from color image and depth image
 */
QImage dc_viewer::calculateNormal( IplImage *clrImg, IplImage *dptImg)
{
	//TODO
	QImage qimg;
	return qimg;
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
	QRegExp re_prefix("^(\\d{4}_\\d{2}_\\d{2}_\\d{2}_\\d{2}_\\d{2}_\\d{3}_\\d{1,5}_\\d{1,5})");
	bool state = false;

	int pos = re_prefix.indexIn(fileInfo.fileName());
	if (pos > -1) {
		prefix = re_prefix.cap(1);
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