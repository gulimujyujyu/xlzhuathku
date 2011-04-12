#include "dc_viewer.h"

dc_viewer::dc_viewer(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	this->fileModel->setRootPath(QDir::currentPath());
}

dc_viewer::~dc_viewer()
{

}
