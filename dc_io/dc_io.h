#ifndef DC_IO_H
#define DC_IO_H

#include <QtGui/QMainWindow>
#include "ui_dc_io.h"

class dc_io : public QMainWindow
{
	Q_OBJECT

public:
	dc_io(QWidget *parent = 0, Qt::WFlags flags = 0);
	~dc_io();

private:
	Ui::dc_ioClass ui;
};

#endif // DC_IO_H
