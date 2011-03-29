#include "dc_io.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	dc_io w;
	w.show();
	return a.exec();
}
