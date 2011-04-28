#ifndef FBX2DEPTH_H
#define FBX2DEPTH_H

#include<QString>
#include<fbxsdk.h>

//Step1: read data
//1.1 load data
void loadData( QString filename, QString savePath);
//TODO: 1.2 assign labels
void assignLabels();

//Step2: rendering
void render(int argc, char *argv[]);
void DisplayCallback();
void KeyboardCallback(unsigned char pKey, int, int);
void MouseCallback(int button, int state, int x, int y);
void MotionCallback(int x, int y);
void TimerCallback(int);

//from x in [-90, 90] y in [0, 360)
bool changeDxDy(int &dx, int &dy);

//Step3: ending works
void destroyAllStuffs();

#endif