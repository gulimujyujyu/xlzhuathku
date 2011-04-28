#include <QtCore/QCoreApplication>
#include <iostream>
#include "fbx2depth.h"
/*
 *	Usage:
 *	fbx2depth.exe MODELPATH SAVEPATH (-roi x y w h)
 */

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	//std::cout << a.argc();
	//for (int i=0; i<a.argc(); i++)
	//{
	//	std::cout << a.argv()[i] << std::endl;
	//}
	if (a.argc() < 3) {
		return -1;
	}
	QString modelPath = a.argv()[1];
	QString savePath = a.argv()[2];
	//Step1: read data
	//1.1 load data
	loadData(modelPath, savePath);
	//TODO: 1.2 assign labels

	//Step2: rendering
	render(argc, argv);
	//2.1 set viewport
	//2.2 render
	//2.3 obtain depthmap/labelmap
	return a.exec();
}
