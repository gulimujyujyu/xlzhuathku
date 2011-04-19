#include "xlpointcloudwidget.h"
#include <QMouseEvent>

// #include <GL/glut.h>

XlPointCloudWidget::XlPointCloudWidget(QWidget *parent)
	: QGLWidget(parent)
{
	setFormat(QGLFormat(QGL::DoubleBuffer| QGL::DepthBuffer));
	this->pointCloud = NULL;
	this->rotateY = this->rotateZ = this->rotateX = 0;
}

XlPointCloudWidget::~XlPointCloudWidget()
{
	if (this->pointCloud) cvReleaseMat(&(this->pointCloud));
}

void XlPointCloudWidget::setColorMap(QImage img)
{
	this->colorMap = img;
	repaint();
}

void XlPointCloudWidget::setPointCloud(CvMat *data)
{	
	pointCloud = cvCloneMat(data);
}

void XlPointCloudWidget::setMeanX(double x)
{
	meanX = x;
}

void XlPointCloudWidget::setMeanY(double y)
{
	meanY = y;
}

void XlPointCloudWidget::setMeanZ(double z)
{
	meanZ = z;
}

void XlPointCloudWidget::draw()
{
	if (this->pointCloud == NULL) {
		return;
	}
	glViewport(0,0,this->width(), this->height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float d = 0.1;
	//glOrtho(minX-d, maxX+d, minY-d, maxY+d, minZ-d, maxZ+d);
	glOrtho(-d, d, -d, d, -d, d);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	int numOfPoints = this->pointCloud->rows;
	int iHeight = this->colorMap.height();
	int iWidth = this->colorMap.width();
	int iy,ix;
	float x,y,z;
	int r,g,b;
	QRgb rgb;

	glRotatef(rotateX, 1.0, 0.0, 0.0);
	glRotatef(rotateY, 0.0, 1.0, 0.0);
	glRotatef(rotateZ, 0.0, 0.0, 1.0);
	glTranslatef(-meanX,-meanY,-meanZ);

	glPointSize(1.5);
	glBegin(GL_POINTS);
	for ( iy=0; iy<iHeight; iy++) {
		for ( ix=0; ix<iWidth; ix++) {
			z = CV_MAT_ELEM(*(this->pointCloud),float,ix+iy*iWidth,2);
			if (fabs(z) <= 1e-8) {
				continue;
			}
			rgb = this->colorMap.pixel(ix,iy);
			r = qRed(rgb);
			g = qGreen(rgb);
			b = qBlue(rgb);			
			glColor3ub(r,g,b);
			x = CV_MAT_ELEM(*(this->pointCloud),float,ix+iy*iWidth,0);
			y = CV_MAT_ELEM(*(this->pointCloud),float,ix+iy*iWidth,1);
			glVertex3f(x,y,z);
		}
	}
	glEnd();
}

void XlPointCloudWidget::initializeGL()
{
	qglClearColor(Qt::black);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void XlPointCloudWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	draw();
}

void XlPointCloudWidget::resizeGL(int w, int h)
{
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat x = GLfloat(w)/h;
	glFrustum(-x,+x,-1.0,+1.0,4.0,15.0);
	glMatrixMode(GL_MODELVIEW);
}

void XlPointCloudWidget::setMinMax(double minX, double minY, double minZ, double maxX, double maxY, double maxZ)
{
	this->minX = minX;
	this->maxX = maxX;
	this->minY = minY;
	this->maxY = maxY;
	this->minZ = minZ;
	this->maxZ = maxZ;
}

void XlPointCloudWidget::mouseMoveEvent(QMouseEvent *ev)
{
	GLfloat dx = GLfloat(ev->x()-lastPos.x()) / width() / ROTATION_PARAM;
	GLfloat dy = GLfloat(ev->y()-lastPos.y()) / height() / ROTATION_PARAM;

	if( ev->buttons() & Qt::LeftButton) {
		rotateX += 180*dy;
		rotateY += 180*dx;
		updateGL();
	} else if( ev->buttons() & Qt::RightButton) {
		rotateX += 180*dy;
		rotateZ += 180*dx;
		updateGL();
	}
}

void XlPointCloudWidget::mousePressEvent(QMouseEvent *ev)
{
	lastPos = ev->pos();
}