/****************************************************************************************

Copyright (C) 2011 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

#ifndef _SET_CAMERA_H
#define _SET_CAMERA_H

void SetCamera(KFbxScene* pScene, 
               KTime& pTime, 
               KFbxAnimLayer* pAnimLayer,
               const KArrayTemplate<KFbxNode*>& pCameraArray,
               int pWindowWidth, int pWindowHeight);

KFbxCamera* GetCurrentCamera(KFbxScene* pScene);

void CameraZoom(KFbxScene* pScene, int pZoomDepth, int pZoomMode);

void CameraOrbit(KFbxScene* pScene, KFbxVector4 lOrigCamPos, double OrigRoll, int dX, int dY);

void CameraPan(KFbxScene* pScene, KFbxVector4 lOrigCamPos, KFbxVector4 lOrigCamCenter, 
			   double OrigRoll, int dX, int dY);

#endif // #ifndef _SET_CAMERA_H






