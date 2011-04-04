#include "xlgldisplay.h"
#include <QImage>
#include <QGLWidget>

/*
 *	Class:	XlGLDisplay
 */
XlGLDisplay::XlGLDisplay(QWidget *parent)
	: QGLWidget(parent)
{
	setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
	this->image = QImage(640,480,QImage::Format_RGB32);
	this->currentRenderMode = RENDER_IMAGE;
	this->textureGenerated = false;
}

XlGLDisplay::~XlGLDisplay()
{

}

void XlGLDisplay::initializeGL()
{
	glClearColor(0,0,0,0);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}

void XlGLDisplay::resizeGL(int width, int height)
{
	//TODO
}

void XlGLDisplay::paintGL()
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (currentRenderMode == RENDER_IMAGE) {
		if ( !(this->textureGenerated))
		{
			initializeTexture();
		}
		drawImage();
	}
	else if(currentRenderMode == RENDER_3D) {
		draw3D();
	}
}

void XlGLDisplay::initializeTexture()
{
	glGenTextures( 1, &texture[0]);
	glBindTexture( GL_TEXTURE_2D, texture[0] );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, image.width(), image.height(),
		GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

	this->textureGenerated = true;
}

void XlGLDisplay::drawImage()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,image.width(),image.height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1.0, 0, 1.0);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glLoadIdentity();
	glPushMatrix();

	glBegin( GL_QUADS );
		glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
		glTexCoord2d(1.0,0.0); glVertex2d(1.0,0.0);
		glTexCoord2d(1.0,1.0); glVertex2d(1.0,1.0);		
		glTexCoord2d(0.0,1.0); glVertex2d(0.0,1.0);
		
	glEnd();
	glPopMatrix();
	glDisable( GL_TEXTURE_2D);
}

void XlGLDisplay::draw3D()
{
	//TODO
}

void XlGLDisplay::putImage(QImage img)
{
	this->image = QGLWidget::convertToGLFormat(img);
	this->textureGenerated = false;
	this->repaint();
}

void XlGLDisplay::setDisplaySettings(const XlGLDisplaySettings & settings) 
{
	this->settings = settings;
}

void XlGLDisplay::setCurrentRenderMode(int mode)
{
	//TODO
}

/*
 *	Class:	XlDisplaySettings 
 */
XlGLDisplaySettings::XlGLDisplaySettings()
{
	this->zoomLevel = 1;
	this->height = 0;
	this->width = 1;
}