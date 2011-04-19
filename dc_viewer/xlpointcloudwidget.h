#ifndef XLPOINTCLOUDWIDGET_H
#define XLPOINTCLOUDWIDGET_H

#include <QGLWidget>
#include <opencv/cv.h>
#include <opencv/highgui.h>

class XlPointCloudWidget : public QGLWidget
{
	Q_OBJECT

public:
	XlPointCloudWidget(QWidget *parent);
	~XlPointCloudWidget();

public: 
	void setColorMap( QImage img);
	void setPointCloud( CvMat *data);
	void setMeanX(double x);
	void setMeanY(double y);
	void setMeanZ(double z);
	void setMinMax(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

private:
	void draw();

private:
	CvMat *pointCloud;
	QImage colorMap;
	double meanX;
	double meanY;
	double meanZ;
	double minX;
	double minY;
	double minZ;
	double maxX;
	double maxY;
	double maxZ;
};

#endif // XLPOINTCLOUDWIDGET_H
