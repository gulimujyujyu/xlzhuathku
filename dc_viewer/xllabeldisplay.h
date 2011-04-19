#ifndef XLLABELDISPLAY_H
#define XLLABELDISPLAY_H

#include <QLabel>

class XlLabelDisplay : public QLabel
{
	Q_OBJECT

public:
	XlLabelDisplay(QWidget *parent);
	~XlLabelDisplay();
	void setTempPath(QString a);

private slots:
	void saveImageLocally();

private:
	QAction *saveAction;

private:
	QString tempPath;
	void createContextMenu();
	
};

#endif // XLLABELDISPLAY_H
