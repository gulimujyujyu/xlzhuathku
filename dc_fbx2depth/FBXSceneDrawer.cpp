#include "FBXSceneDrawer.h"
#include "GL/glut.h"

FBXSceneDrawer::FBXSceneDrawer(void)
{
}

FBXSceneDrawer::~FBXSceneDrawer(void)
{
}

bool FBXSceneDrawer::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glViewport(0,0,this->width, this->height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float d = 0.1;
	//glOrtho(minX-d, maxX+d, minY-d, maxY+d, minZ-d, maxZ+d);
	glOrtho(-d, d, -d, d, -d, d);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//start
	glPointSize(1.5);
	glBegin(GL_POINTS);
		glVertex3f( 0.0f, 0.0f, 0.0f);
	glEnd();

	return true;
}

void FBXSceneDrawer::setWidthAndHeight(int width, int height)
{
	this->width = width;
	this->height = height;
}