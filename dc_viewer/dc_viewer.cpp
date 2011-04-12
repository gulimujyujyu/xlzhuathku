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

	//TODO
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
	QImage colorData = this->iplimage2qimage(this->colorImage);
	QImage depthData = this->iplimage2qimage(this->depthImage);
	
	ui.colorLabel->setPixmap(QPixmap::fromImage(colorData));
	ui.depthLabel->setPixmap(QPixmap::fromImage(depthData));
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