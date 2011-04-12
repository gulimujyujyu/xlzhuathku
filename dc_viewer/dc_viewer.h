#ifndef DC_VIEWER_H
#define DC_VIEWER_H

#include <QtGui/QMainWindow>
#include "ui_dc_viewer.h"

class dc_viewer : public QMainWindow
{
	Q_OBJECT

public:
	dc_viewer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~dc_viewer();

private:
	Ui::dc_viewerClass ui;
};

#endif // DC_VIEWER_H
