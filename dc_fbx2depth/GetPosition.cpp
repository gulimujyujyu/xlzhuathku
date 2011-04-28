/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/
 
/////////////////////////////////////////////////////////////////////////
//
// This file contains the functions to get the global 
// position of a node for a given time in the current animation stack.
//
/////////////////////////////////////////////////////////////////////////

#include "GetPosition.h"

// Get the global position of the node for the current pose.
// If the specified node is not part of the pose or no pose is specified, get its
// global position at the current time.
KFbxXMatrix GetGlobalPosition(KFbxNode* pNode, const KTime& pTime, KFbxPose* pPose, KFbxXMatrix* pParentGlobalPosition)
{
    KFbxXMatrix lGlobalPosition;
    bool        lPositionFound = false;

    if (pPose)
    {
        int lNodeIndex = pPose->Find(pNode);

        if (lNodeIndex > -1)
        {
            // The bind pose is always a global matrix.
            // If we have a rest pose, we need to check if it is
            // stored in global or local space.
            if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
            {
                lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
            }
            else
            {
                // We have a local matrix, we need to convert it to
                // a global space matrix.
                KFbxXMatrix lParentGlobalPosition;

                if (pParentGlobalPosition)
                {
                    lParentGlobalPosition = *pParentGlobalPosition;
                }
                else
                {
                    if (pNode->GetParent())
                    {
                        lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
                    }
                }

                KFbxXMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
                lGlobalPosition = lParentGlobalPosition * lLocalPosition;
            }

            lPositionFound = true;
        }
    }

    if (!lPositionFound)
    {
        // There is no pose entry for that node, get the current global position instead.

        // Ideally this would use parent global position and local position to compute the global position.
        // Unfortunately the equation 
        //    lGlobalPosition = pParentGlobalPosition * lLocalPosition
        // does not hold when inheritance type is other than "Parent" (RSrs).
        // To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
        lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
    }

    return lGlobalPosition;
}

// Get the matrix of the given pose
KFbxXMatrix GetPoseMatrix(KFbxPose* pPose, int pNodeIndex)
{
    KFbxXMatrix lPoseMatrix;
    KFbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

    memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

    return lPoseMatrix;
}

// Get the geometry offset to a node. It is never inherited by the children.
KFbxXMatrix GetGeometry(KFbxNode* pNode)
{
    const KFbxVector4 lT = pNode->GetGeometricTranslation(KFbxNode::eSOURCE_SET);
    const KFbxVector4 lR = pNode->GetGeometricRotation(KFbxNode::eSOURCE_SET);
    const KFbxVector4 lS = pNode->GetGeometricScaling(KFbxNode::eSOURCE_SET);

    return KFbxXMatrix(lT, lR, lS);
}

