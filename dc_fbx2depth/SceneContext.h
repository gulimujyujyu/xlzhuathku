/****************************************************************************************

Copyright (C) 2011 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

#ifndef _SCENE_CONTEXT_H
#define _SCENE_CONTEXT_H

#include "GlFunctions.h"
#include <QString>

class DrawText;

// This class is responsive for loading files and recording current status as
// a bridge between window system such as GLUT or Qt and a specific FBX scene.
class SceneContext
{
public:
    enum Status
    {
        UNLOADED,               // Unload file or load failure;
        MUST_BE_LOADED,         // Ready for loading file;
        MUST_BE_REFRESHED,      // Something changed and redraw needed;
        REFRESHED               // No redraw needed.
    };
    Status GetStatus() const { return mStatus; }

    // Initialize with a .FBX, .DAE or .OBJ file name and current window size.
    SceneContext(QString pFileName, QString savePath, int pWindowWidth, int pWindowHeight, bool pSupportVBO);
    ~SceneContext();

    // Return the FBX scene for more informations.
    const KFbxScene * GetScene() const { return mScene; }
    // Load the FBX or COLLADA file into memory.
    bool LoadFile();

	// Set ROI
	void setROI(int x, int y, int w, int h);

    // The time period for one frame.
    const KTime GetFrameTime() const { return mFrameTime; }

    // Call this method when redraw is needed.
    bool OnDisplay();
    // Call this method when window size is changed.
    void OnReshape(int pWidth, int pHeight);
    // Call this method when keyboard input occurs.
    void OnKeyboard(unsigned char pKey, int pX, int pY);
    // Call this method when mouse buttons are pushed or released.
    void OnMouse(int pButton, int pState, int pX, int pY);
    // Call this method when mouse is moved.
    void OnMouseMotion(int pX, int pY);
    // Call this method when timer is finished, for animation display.
    void OnTimerClick() const;

    // Methods for creating menus.
    // Get all the cameras in current scene, including producer cameras.
    const KArrayTemplate<KFbxNode *> & GetCameraArray() const { return mCameraArray; }
    // Get all the animation stack names in current scene.
    const KArrayTemplate<KString *> & GetAnimStackNameArray() const { return mAnimStackNameArray; }
    // Get all the pose in current scene.
    const KArrayTemplate<KFbxPose *> & GetPoseArray() const { return mPoseArray; }

    // The input index is corresponding to the array returned from GetAnimStackNameArray.
    bool SetCurrentAnimStack(int pIndex);
    // Set the current camera with its name.
    bool SetCurrentCamera(const char * pCameraName);
    // The input index is corresponding to the array returned from GetPoseArray.
    bool SetCurrentPoseIndex(int pPoseIndex);
    // Set the currently selected node from external window system.
    void SetSelectedNode(KFbxNode * pSelectedNode);
    // Set the shading mode, wire-frame or shaded.
    void SetShadingMode(ShadingMode pMode);

    // Pause the animation.
    void SetPause(bool pPause) { mPause = pPause; }
    // Check whether the animation is paused.
    bool GetPause() const { return mPause; }


    enum CameraZoomMode
    {
        ZOOM_FOCAL_LENGTH,
        ZOOM_POSITION
    };
    CameraZoomMode  GetZoomMode()        { return mCameraZoomMode; }
    void            SetZoomMode( CameraZoomMode pZoomMode);       

private:
    // Display information about current status in the left-up corner of the window.
    void DisplayWindowMessage();
    // Display a X-Z grid.
    void DisplayGrid(const KFbxXMatrix & pTransform);
	// Save depth map
	void SaveDepthMap();

    enum CameraStatus
    {
        CAMERA_NOTHING,
        CAMERA_ORBIT,
        CAMERA_ZOOM,
        CAMERA_PAN
    };

	int mROI_x;
	int mROI_y;
	int mROI_w;
	int mROI_h;
	const char * mFileName;
	const char * mSavePath;
	QByteArray mFileNameByte;
	QByteArray mSavePathByte;
    QString qFileName;
	QString qSavePath;
    mutable Status mStatus;
    mutable KString mWindowMessage;

    KFbxSdkManager * mSdkManager;
    KFbxScene * mScene;
    KFbxImporter * mImporter;
    KFbxAnimLayer * mCurrentAnimLayer;
    KFbxNode * mSelectedNode;

    int mPoseIndex;
    KArrayTemplate<KString*> mAnimStackNameArray;
    KArrayTemplate<KFbxNode*> mCameraArray;
    KArrayTemplate<KFbxPose*> mPoseArray;

    mutable KTime mFrameTime, mStart, mStop, mCurrentTime;

    // Data for camera manipulation
    mutable int mLastX, mLastY;
    mutable KFbxVector4 mCamPosition, mCamCenter;
    mutable double mRoll;
    mutable CameraStatus mCameraStatus;

    bool mPause;
    ShadingMode mShadingMode;
    bool mSupportVBO;

    //camera zoom mode
    CameraZoomMode mCameraZoomMode;

    int mWindowWidth, mWindowHeight;
    // Utility class for draw text in OpenGL.
    DrawText * mDrawText;
};

// Initialize GLEW, must be called after the window is created.
bool InitializeOpenGL();

#endif // _SCENE_CONTEXT_H

