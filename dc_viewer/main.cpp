#include "dc_viewer.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	dc_viewer w;
	w.show();
	return a.exec();
}
