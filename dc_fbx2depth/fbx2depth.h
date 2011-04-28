#ifndef FBX2DEPTH_H
#define FBX2DEPTH_H

#include<QString>
#include<fbxsdk.h>

//Step1: read data
//1.1 load data
void loadData( QString filename);
//TODO: 1.2 assign labels
void assignLabels();

//Step2: rendering
void render(int argc, char *argv[]);
void DisplayCallback();
/*
//2.1 set viewport
void setViewPort( int i, int j);
//2.2 render
void renderHand();
//2.3 obtain depthmap/labelmap
bool captureDepthAndColor( QString path);
*/

//Step3: ending works
void destroyAllStuffs();

#endif