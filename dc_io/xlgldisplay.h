#ifndef XLGLDISPLAY_H
#define XLGLDISPLAY_H

#include <QGLWidget>
#include <QImage>
#include <QLabel>
#include <QVBoxLayout>

class XlGLDisplaySettings {
public:
	XlGLDisplaySettings();

	int height;
	int width;
	double zoomLevel;
};

#define RENDER_IMAGE 1
#define RENDER_3D 2

class XlGLDisplay : public QGLWidget
{
	Q_OBJECT

public:
	XlGLDisplay(QWidget *parent);
	~XlGLDisplay();

	void setDisplaySettings(const XlGLDisplaySettings & settings);
	void putImage(QImage img);
	void setCurrentRenderMode(int mode);

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

private:
	GLuint texture[1];
	bool textureGenerated;
	void initializeTexture();
	void drawImage();
	void draw3D();
	QImage image;
	XlGLDisplaySettings settings;

	int currentRenderMode;
};

#endif // XLDISPLAY_H
