#ifndef XLPOINTCLOUDWIDGET_H
#define XLPOINTCLOUDWIDGET_H

#include <QGLWidget>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#define ROTATION_PARAM 100

class XlPointCloudWidget : public QGLWidget
{
	Q_OBJECT

public:
	XlPointCloudWidget(QWidget *parent);
	~XlPointCloudWidget();
	void setColorMap( QImage img);
	void setPointCloud( CvMat *data);
	void setMeanX(double x);
	void setMeanY(double y);
	void setMeanZ(double z);
	void setMinMax(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
	void setTempPath(QString a);

private slots:
	void saveImageLocally();
	void zoomIn();
	void zoomOut();
	void resetAllParam();

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void wheelEvent(QWheelEvent *ev);

private:
	void draw();
	void createContextMenu();

private:
	QAction *saveAction;
	QAction *zoomInAction;
	QAction *zoomOutAction;
	QAction *resetAction;

	//variables
	float zoomLevel;
	QString tempPath;
	QPoint lastPos;
	GLfloat rotateX;
	GLfloat rotateY;
	GLfloat rotateZ;
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
