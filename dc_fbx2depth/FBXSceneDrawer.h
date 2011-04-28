#ifndef FBX_SCENE_DRAWER_H
#define FBX_SCENE_DRAWER_H
#include <fbxsdk.h>

class FBXSceneDrawer
{
public:
	FBXSceneDrawer(void);
	~FBXSceneDrawer(void);
	bool OnDisplay();
	void setWidthAndHeight(int width, int height);

public:
	//globals
	KFbxSdkManager* lSdkManager;
	KFbxScene* lScene;
	int width;
	int height;
};

#endif