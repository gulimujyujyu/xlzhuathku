/****************************************************************************************

Copyright (C) 2011 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

#ifndef _GET_POSITION_H
#define _GET_POSITION_H

#include <fbxsdk.h>

KFbxXMatrix GetGlobalPosition(KFbxNode* pNode, 
							  const KTime& pTime, 
							  KFbxPose* pPose = NULL,
							  KFbxXMatrix* pParentGlobalPosition = NULL);
KFbxXMatrix GetPoseMatrix(KFbxPose* pPose, 
                          int pNodeIndex);
KFbxXMatrix GetGeometry(KFbxNode* pNode);

#endif // #ifndef _GET_POSITION_H



