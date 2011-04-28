/****************************************************************************************

Copyright (C) 2011 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

#ifndef _DRAW_SCENE_H
#define _DRAW_SCENE_H

#include "GlFunctions.h"

void InitializeLights(const KFbxScene* pScene, const KTime & pTime, KFbxPose* pPose = NULL);

void DrawNodeRecursive(KFbxNode* pNode, KTime& pTime, KFbxAnimLayer* pAnimLayer, 
                       KFbxXMatrix& pParentGlobalPosition,
                       KFbxPose* pPose, ShadingMode pShadingMode);

#endif // #ifndef _DRAW_SCENE_H






