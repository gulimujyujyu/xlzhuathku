/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

#include "DrawScene.h"
#include "SceneCache.h"
#include "GetPosition.h"

void DrawNode(KFbxNode* pNode, 
              KTime& lTime, 
              KFbxAnimLayer * pAnimLayer,
              KFbxXMatrix& pParentGlobalPosition,
              KFbxXMatrix& pGlobalPosition,
              KFbxPose* pPose,
              ShadingMode pShadingMode);
void DrawMarker(KFbxXMatrix& pGlobalPosition);
void DrawSkeleton(KFbxNode* pNode, 
                  KFbxXMatrix& pParentGlobalPosition, 
                  KFbxXMatrix& pGlobalPosition);
void DrawMesh(KFbxNode* pNode, KTime& pTime, KFbxAnimLayer* pAnimLayer,
              KFbxXMatrix& pGlobalPosition, KFbxPose* pPose, ShadingMode pShadingMode);
void ComputeShapeDeformation(KFbxNode* pNode,
                             KFbxMesh* pMesh, 
                             KTime& pTime, 
                             KFbxAnimLayer * pAnimLayer,
                             KFbxVector4* pVertexArray);
void ComputeClusterDeformation(KFbxXMatrix& pGlobalPosition, 
							   KFbxMesh* pMesh,
							   KFbxCluster* pCluster, 
							   KFbxXMatrix& pVertexTransformMatrix,
							   KTime pTime, 
							   KFbxPose* pPose);
void ComputeLinearDeformation(KFbxXMatrix& pGlobalPosition, 
							  KFbxMesh* pMesh, 
							  KTime& pTime, 
							  KFbxVector4* pVertexArray,
							  KFbxPose* pPose);
void ComputeDualQuaternionDeformation(KFbxXMatrix& pGlobalPosition, 
									  KFbxMesh* pMesh, 
									  KTime& pTime, 
									  KFbxVector4* pVertexArray,
									  KFbxPose* pPose);
void ComputeSkinDeformation(KFbxXMatrix& pGlobalPosition, 
							KFbxMesh* pMesh, 
							KTime& pTime, 
							KFbxVector4* pVertexArray,
							KFbxPose* pPose);
void ReadVertexCacheData(KFbxMesh* pMesh, 
                         KTime& pTime, 
                         KFbxVector4* pVertexArray);
void DrawCamera(KFbxNode* pNode, 
                KTime& pTime, 
                KFbxAnimLayer* pAnimLayer,
                KFbxXMatrix& pGlobalPosition);
void DrawLight(const KFbxNode* pNode, const KTime& pTime, const KFbxXMatrix& pGlobalPosition);
void DrawNull(KFbxXMatrix& pGlobalPosition);
void MatrixScale(KFbxXMatrix& pMatrix, double pValue);
void MatrixAddToDiagonal(KFbxXMatrix& pMatrix, double pValue);
void MatrixAdd(KFbxXMatrix& pDstMatrix, KFbxXMatrix& pSrcMatrix);

void InitializeLights(const KFbxScene* pScene, const KTime & pTime, KFbxPose* pPose)
{
    // Set ambient light. Turn on light0 and set its attributes to default (white directional light in Z axis).
    // If the scene contains at least one light, the attributes of light0 will be overridden.
    LightCache::IntializeEnvironment(pScene->GetGlobalSettings().GetAmbientColor());

    // Setting the lights before drawing the whole scene
    const int lLightCount = KFbxGetSrcCount<KFbxLight>(pScene);
    for (int lLightIndex = 0; lLightIndex < lLightCount; ++lLightIndex)
    {
        KFbxLight * lLight = KFbxGetSrc<KFbxLight>(pScene, lLightIndex);
        KFbxNode * lNode = lLight->GetNode();
        if (lNode)
{
            KFbxXMatrix lGlobalPosition = GetGlobalPosition(lNode, pTime, pPose);
            KFbxXMatrix lGeometryOffset = GetGeometry(lNode);
            KFbxXMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;
            DrawLight(lNode, pTime, lGlobalOffPosition);
    }
    }
}

// Draw recursively each node of the scene. To avoid recomputing 
// uselessly the global positions, the global position of each 
// node is passed to it's children while browsing the node tree.
// If the node is part of the given pose for the current scene,
// it will be drawn at the position specified in the pose, Otherwise
// it will be drawn at the given time.
void DrawNodeRecursive(KFbxNode* pNode, KTime& pTime, KFbxAnimLayer* pAnimLayer,
                       KFbxXMatrix& pParentGlobalPosition, KFbxPose* pPose,
                       ShadingMode pShadingMode)
{
    KFbxXMatrix lGlobalPosition = GetGlobalPosition(pNode, pTime, pPose, &pParentGlobalPosition);

    if (pNode->GetNodeAttribute())
    {
    // Geometry offset.
    // it is not inherited by the children.
    KFbxXMatrix lGeometryOffset = GetGeometry(pNode);
    KFbxXMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;

        DrawNode(pNode, pTime, pAnimLayer, pParentGlobalPosition, lGlobalOffPosition, pPose, pShadingMode);
    }

    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
    {
        DrawNodeRecursive(pNode->GetChild(lChildIndex), pTime, pAnimLayer, lGlobalPosition, pPose, pShadingMode);
    }
}

// Draw the node following the content of it's node attribute.
void DrawNode(KFbxNode* pNode, 
              KTime& pTime, 
              KFbxAnimLayer* pAnimLayer,
              KFbxXMatrix& pParentGlobalPosition,
              KFbxXMatrix& pGlobalPosition,
              KFbxPose* pPose, ShadingMode pShadingMode)
{
    KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

    if (lNodeAttribute)
    {
        // All lights has been processed before the whole scene because they influence every geometry.
        if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eMARKER && NEED_DRAW_MAKER)
        {
            DrawMarker(pGlobalPosition);
        }
        else if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eSKELETON && NEED_DRAW_SKELETON)
        {
            DrawSkeleton(pNode, pParentGlobalPosition, pGlobalPosition);
        }
        // NURBS and patch have been converted into triangluation meshes.
        else if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eMESH && NEED_DRAW_MESH)
        {
            DrawMesh(pNode, pTime, pAnimLayer, pGlobalPosition, pPose, pShadingMode);
        }
        else if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eCAMERA && NEED_DRAW_CAMERA)
        {
            DrawCamera(pNode, pTime, pAnimLayer, pGlobalPosition);
        }
        else if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eNULL && NEED_DRAW_NULL)
        {
            DrawNull(pGlobalPosition);
        }
    }
    else if(NEED_DRAW_NULL)
    {
        // Draw a Null for nodes without attribute.
        DrawNull(pGlobalPosition);
    }
}


// Draw a small box where the node is located.
void DrawMarker(KFbxXMatrix& pGlobalPosition)
{
    GlDrawMarker(pGlobalPosition);  
}


// Draw a limb between the node and its parent.
void DrawSkeleton(KFbxNode* pNode, KFbxXMatrix& pParentGlobalPosition, KFbxXMatrix& pGlobalPosition)
{
    KFbxSkeleton* lSkeleton = (KFbxSkeleton*) pNode->GetNodeAttribute();

    // Only draw the skeleton if it's a limb node and if 
    // the parent also has an attribute of type skeleton.
    if (lSkeleton->GetSkeletonType() == KFbxSkeleton::eLIMB_NODE &&
        pNode->GetParent() &&
        pNode->GetParent()->GetNodeAttribute() &&
        pNode->GetParent()->GetNodeAttribute()->GetAttributeType() == KFbxNodeAttribute::eSKELETON)
    {
        GlDrawLimbNode(pParentGlobalPosition, pGlobalPosition); 
    }
}


// Draw the vertices of a mesh.
void DrawMesh(KFbxNode* pNode, KTime& pTime, KFbxAnimLayer* pAnimLayer,
              KFbxXMatrix& pGlobalPosition, KFbxPose* pPose, ShadingMode pShadingMode)
{
    KFbxMesh* lMesh = pNode->GetMesh();
    const int lVertexCount = lMesh->GetControlPointsCount();

    // No vertex to draw.
    if (lVertexCount == 0)
    {
        return;
    }

    const VBOMesh * lMeshCache = static_cast<const VBOMesh *>(lMesh->GetUserDataPtr());

    // If it has some defomer connection, update the vertices position
    const bool lHasVertexCache = lMesh->GetDeformerCount(KFbxDeformer::eVERTEX_CACHE) &&
        (static_cast<KFbxVertexCacheDeformer*>(lMesh->GetDeformer(0, KFbxDeformer::eVERTEX_CACHE)))->IsActive();
    const bool lHasShape = lMesh->GetShapeCount() > 0;
    const bool lHasSkin = lMesh->GetDeformerCount(KFbxDeformer::eSKIN) > 0;
    const bool lHasDeformation = lHasVertexCache || lHasShape || lHasSkin;

    KFbxVector4* lVertexArray = NULL;
    if (!lMeshCache || lHasDeformation)
    {
        lVertexArray = new KFbxVector4[lVertexCount];
        memcpy(lVertexArray, lMesh->GetControlPoints(), lVertexCount * sizeof(KFbxVector4));
    }

    if (lHasDeformation)
    {
        // Active vertex cache deformer will overwrite any other deformer
        if (lHasVertexCache)
        {
            ReadVertexCacheData(lMesh, pTime, lVertexArray);
        }
        else
        {
            if (lHasShape)
            {
                // Deform the vertex array with the shapes.
                ComputeShapeDeformation(pNode, lMesh, pTime, pAnimLayer, lVertexArray);
            }

            //we need to get the number of clusters
            const int lSkinCount = lMesh->GetDeformerCount(KFbxDeformer::eSKIN);
            int lClusterCount = 0;
            for (int lSkinIndex = 0; lSkinIndex < lSkinCount; ++lSkinIndex)
            {
                lClusterCount += ((KFbxSkin *)(lMesh->GetDeformer(lSkinIndex, KFbxDeformer::eSKIN)))->GetClusterCount();
            }
            if (lClusterCount)
            {
                // Deform the vertex array with the skin deformer.
                ComputeSkinDeformation(pGlobalPosition, lMesh, pTime, lVertexArray, pPose);
            }
        }

        if (lMeshCache)
            lMeshCache->UpdateVertexPosition(lMesh, lVertexArray);
    }

    glPushMatrix();
    glMultMatrixd((const double*)pGlobalPosition);

    if (lMeshCache)
    {
        lMeshCache->BeginDraw(pShadingMode);
        const int lSubMeshCount = lMeshCache->GetSubMeshCount();
        for (int lIndex = 0; lIndex < lSubMeshCount; ++lIndex)
        {
            if (pShadingMode == SHADING_MODE_SHADED)
            {
                const KFbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lIndex);
                if (lMaterial)
                {
                    const MaterialCache * lMaterialCache = static_cast<const MaterialCache *>(lMaterial->GetUserDataPtr());
                    if (lMaterialCache)
                    {
                        lMaterialCache->SetCurrentMaterial();
                    }
                }
                else
                {
                    // Draw green for faces without material
                    MaterialCache::SetDefaultMaterial();
                }
            }

            lMeshCache->Draw(lIndex, pShadingMode);
        }
        lMeshCache->EndDraw(pShadingMode);
    }
    else
    {
        // OpenGL driver is too lower and use Immediate Mode
        glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
        const int lPolygonCount = lMesh->GetPolygonCount();
        for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; lPolygonIndex++)
        {
            const int lVerticeCount = lMesh->GetPolygonSize(lPolygonIndex);
            glBegin(GL_LINE_LOOP);
            for (int lVerticeIndex = 0; lVerticeIndex < lVerticeCount; lVerticeIndex++)
            {
                glVertex3dv((GLdouble *)lVertexArray[lMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex)]);
            }
            glEnd();
        }
    }

    glPopMatrix();

    delete [] lVertexArray;
}


// Deform the vertex array with the shapes contained in the mesh.
void ComputeShapeDeformation(KFbxNode* pNode,
                             KFbxMesh* pMesh, 
                             KTime& pTime, 
                             KFbxAnimLayer * pAnimLayer,
                             KFbxVector4* pVertexArray)
{
    int lVertexCount = pMesh->GetControlPointsCount();

    KFbxVector4* lSrcVertexArray = pVertexArray;
    KFbxVector4* lDstVertexArray = new KFbxVector4[lVertexCount];
    memcpy(lDstVertexArray, pVertexArray, lVertexCount * sizeof(KFbxVector4));

	int lBlendShapeDeformerCount = pMesh->GetDeformerCount(KFbxDeformer::eBLENDSHAPE);
	for(int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
	{
		KFbxBlendShape* lBlendShape = (KFbxBlendShape*)pMesh->GetDeformer(lBlendShapeIndex, KFbxDeformer::eBLENDSHAPE);

		int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
		for(int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; ++lChannelIndex)
		{
			KFbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);

			if(lChannel)
			{
				// Get the percentage of influence of the shape.
				KFbxAnimCurve* lFCurve = pMesh->GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer);
				if (!lFCurve) continue;
				double lWeight = lFCurve->Evaluate(pTime);

				//Find which shape should we use according to the weight.
				int lShapeCount = lChannel->GetTargetShapeCount();
				double* lFullWeights = lChannel->GetTargetShapeFullWeights();
				for(int lShapeIndex = 0; lShapeIndex<lShapeCount; ++lShapeIndex)
				{
					KFbxShape* lShape = NULL;
					if(lWeight > 0 && lWeight < lFullWeights[0])
					{
						lShape = lChannel->GetTargetShape(0);
					}
					if(lWeight > lFullWeights[lShapeIndex] && lWeight < lFullWeights[lShapeIndex+1])
					{
						lShape = lChannel->GetTargetShape(lShapeIndex+1);
					}

					if(lShape)
					{				
						for (int j = 0; j < lVertexCount; j++)
						{
							// Add the influence of the shape vertex to the mesh vertex.
							KFbxVector4 lInfluence = (lShape->GetControlPoints()[j] - lSrcVertexArray[j]) * lWeight * 0.01;
							lDstVertexArray[j] += lInfluence;
						}						
					}
				}//For each target shape
			}//If lChannel is valid
		}//For each blend shape channel
	}//For each blend shape deformer

    memcpy(pVertexArray, lDstVertexArray, lVertexCount * sizeof(KFbxVector4));

    delete [] lDstVertexArray;
}

//Compute the transform matrix that the cluster will transform the vertex.
void ComputeClusterDeformation(KFbxXMatrix& pGlobalPosition, 
							   KFbxMesh* pMesh,
							   KFbxCluster* pCluster, 
							   KFbxXMatrix& pVertexTransformMatrix,
							   KTime pTime, 
							   KFbxPose* pPose)
{
    KFbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();

	KFbxXMatrix lReferenceGlobalInitPosition;
	KFbxXMatrix lReferenceGlobalCurrentPosition;
	KFbxXMatrix lAssociateGlobalInitPosition;
	KFbxXMatrix lAssociateGlobalCurrentPosition;
	KFbxXMatrix lClusterGlobalInitPosition;
	KFbxXMatrix lClusterGlobalCurrentPosition;

	KFbxXMatrix lReferenceGeometry;
	KFbxXMatrix lAssociateGeometry;
	KFbxXMatrix lClusterGeometry;

	KFbxXMatrix lClusterRelativeInitPosition;
	KFbxXMatrix lClusterRelativeCurrentPositionInverse;
	
	if (lClusterMode == KFbxLink::eADDITIVE && pCluster->GetAssociateModel())
	{
		pCluster->GetTransformAssociateModelMatrix(lAssociateGlobalInitPosition);
		// Geometric transform of the model
		lAssociateGeometry = GetGeometry(pCluster->GetAssociateModel());
		lAssociateGlobalInitPosition *= lAssociateGeometry;
		lAssociateGlobalCurrentPosition = GetGlobalPosition(pCluster->GetAssociateModel(), pTime, pPose);

		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;
		lReferenceGlobalCurrentPosition = pGlobalPosition;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		// Multiply lClusterGlobalInitPosition by Geometric Transformation
		lClusterGeometry = GetGeometry(pCluster->GetLink());
		lClusterGlobalInitPosition *= lClusterGeometry;
		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the shift of the link relative to the reference.
		//ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
		pVertexTransformMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse() *
			lClusterGlobalCurrentPosition * lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
	}
	else
	{
		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		lReferenceGlobalCurrentPosition = pGlobalPosition;
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the initial position of the link relative to the reference.
		lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

		// Compute the current position of the link relative to the reference.
		lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

		// Compute the shift of the link relative to the reference.
		pVertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
	}
}

// Deform the vertex array in classic linear way.
void ComputeLinearDeformation(KFbxXMatrix& pGlobalPosition, 
                               KFbxMesh* pMesh, 
                               KTime& pTime, 
                               KFbxVector4* pVertexArray,
							   KFbxPose* pPose)
{
	// All the links must have the same link mode.
	KFbxCluster::ELinkMode lClusterMode = ((KFbxSkin*)pMesh->GetDeformer(0, KFbxDeformer::eSKIN))->GetCluster(0)->GetLinkMode();

	int lVertexCount = pMesh->GetControlPointsCount();
	KFbxXMatrix* lClusterDeformation = new KFbxXMatrix[lVertexCount];
	memset(lClusterDeformation, 0, lVertexCount * sizeof(KFbxXMatrix));

	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));

	if (lClusterMode == KFbxCluster::eADDITIVE)
	{
		for (int i = 0; i < lVertexCount; ++i)
		{
			lClusterDeformation[i].SetIdentity();
		}
	}

	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	int lSkinCount = pMesh->GetDeformerCount(KFbxDeformer::eSKIN);
	for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
	{
		KFbxSkin * lSkinDeformer = (KFbxSkin *)pMesh->GetDeformer(lSkinIndex, KFbxDeformer::eSKIN);
		
		int lClusterCount = lSkinDeformer->GetClusterCount();
		for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
		{
			KFbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;

			KFbxXMatrix lVertexTransformMatrix;
			ComputeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix, pTime, pPose);

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k) 
			{            
				int lIndex = lCluster->GetControlPointIndices()[k];

				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
				{
					continue;
				}

				// Compute the influence of the link on the vertex.
				KFbxXMatrix lInfluence = lVertexTransformMatrix;
				MatrixScale(lInfluence, lWeight);

				if (lClusterMode == KFbxCluster::eADDITIVE)
				{    
					// Multiply with the product of the deformations on the vertex.
					MatrixAddToDiagonal(lInfluence, 1.0 - lWeight);
					lClusterDeformation[lIndex] = lInfluence * lClusterDeformation[lIndex];

					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == KFbxLink::eNORMALIZE || lLinkMode == KFbxLink::eTOTAL1
				{
					// Add to the sum of the deformations on the vertex.
					MatrixAdd(lClusterDeformation[lIndex], lInfluence);

					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex			
		}//lClusterCount
	}

	//Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
	for (int i = 0; i < lVertexCount; i++) 
	{
		KFbxVector4 lSrcVertex = pVertexArray[i];
		KFbxVector4& lDstVertex = pVertexArray[i];
		double lWeight = lClusterWeight[i];

		// Deform the vertex if there was at least a link with an influence on the vertex,
		if (lWeight != 0.0) 
		{
			lDstVertex = lClusterDeformation[i].MultT(lSrcVertex);
			if (lClusterMode == KFbxCluster::eNORMALIZE)
			{
				// In the normalized link mode, a vertex is always totally influenced by the links. 
				lDstVertex /= lWeight;
			}
			else if (lClusterMode == KFbxCluster::eTOTAL1)
			{
				// In the total 1 link mode, a vertex can be partially influenced by the links. 
				lSrcVertex *= (1.0 - lWeight);
				lDstVertex += lSrcVertex;
			}
		} 
	}

	delete [] lClusterDeformation;
	delete [] lClusterWeight;
}

// Deform the vertex array in Dual Quaternion Skinning way.
void ComputeDualQuaternionDeformation(KFbxXMatrix& pGlobalPosition, 
									 KFbxMesh* pMesh, 
									 KTime& pTime, 
									 KFbxVector4* pVertexArray,
									 KFbxPose* pPose)
{
	// All the links must have the same link mode.
	KFbxCluster::ELinkMode lClusterMode = ((KFbxSkin*)pMesh->GetDeformer(0, KFbxDeformer::eSKIN))->GetCluster(0)->GetLinkMode();

	int lVertexCount = pMesh->GetControlPointsCount();
	int lSkinCount = pMesh->GetDeformerCount(KFbxDeformer::eSKIN);

	KFbxDualQuaternion* lDQClusterDeformation = new KFbxDualQuaternion[lVertexCount];
	memset(lDQClusterDeformation, 0, lVertexCount * sizeof(KFbxDualQuaternion));

	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));

	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
	{
		KFbxSkin * lSkinDeformer = (KFbxSkin *)pMesh->GetDeformer(lSkinIndex, KFbxDeformer::eSKIN);
		int lClusterCount = lSkinDeformer->GetClusterCount();
		for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
		{
			KFbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;

			KFbxXMatrix lVertexTransformMatrix;
			ComputeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix, pTime, pPose);

			KFbxQuaternion lQ = lVertexTransformMatrix.GetQ();
			KFbxVector4 lT = lVertexTransformMatrix.GetT();
			KFbxDualQuaternion lDualQuaternion(lQ, lT);

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k) 
			{ 
				int lIndex = lCluster->GetControlPointIndices()[k];

				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
					continue;

				// Compute the influence of the link on the vertex.
				KFbxDualQuaternion lInfluence = lDualQuaternion * lWeight;
				if (lClusterMode == KFbxCluster::eADDITIVE)
				{    
					// Simply influenced by the dual quaternion.
					lDQClusterDeformation[lIndex] = lInfluence;

					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == KFbxLink::eNORMALIZE || lLinkMode == KFbxLink::eTOTAL1
				{
					if(lClusterIndex == 0)
					{
						lDQClusterDeformation[lIndex] = lInfluence;
					}
					else
					{
						// Add to the sum of the deformations on the vertex.
						// Make sure the deformation is accumulated in the same rotation direction. 
						// Use dot product to judge the sign.
						double lSign = lDQClusterDeformation[lIndex].GetFirstQuaternion().DotProduct(lDualQuaternion.GetFirstQuaternion());
						if( lSign >= 0.0 )
						{
							lDQClusterDeformation[lIndex] += lInfluence;
						}
						else
						{
							lDQClusterDeformation[lIndex] -= lInfluence;
						}
					}
					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex
		}//lClusterCount
	}

	//Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
	for (int i = 0; i < lVertexCount; i++) 
	{
		KFbxVector4 lSrcVertex = pVertexArray[i];
		KFbxVector4& lDstVertex = pVertexArray[i];
		double lWeightSum = lClusterWeight[i];

		// Deform the vertex if there was at least a link with an influence on the vertex,
		if (lWeightSum != 0.0) 
		{
			lDQClusterDeformation[i].Normalize();
			lDstVertex = lDQClusterDeformation[i].Deform(lDstVertex);

			if (lClusterMode == KFbxCluster::eNORMALIZE)
			{
				// In the normalized link mode, a vertex is always totally influenced by the links. 
				lDstVertex /= lWeightSum;
			}
			else if (lClusterMode == KFbxCluster::eTOTAL1)
			{
				// In the total 1 link mode, a vertex can be partially influenced by the links. 
				lSrcVertex *= (1.0 - lWeightSum);
				lDstVertex += lSrcVertex;
			}
		} 
	}

	delete [] lDQClusterDeformation;
	delete [] lClusterWeight;
}

// Deform the vertex array according to the links contained in the mesh and the skinning type.
void ComputeSkinDeformation(KFbxXMatrix& pGlobalPosition, 
									 KFbxMesh* pMesh, 
									 KTime& pTime, 
									 KFbxVector4* pVertexArray,
									 KFbxPose* pPose)
{
	KFbxSkin * lSkinDeformer = (KFbxSkin *)pMesh->GetDeformer(0, KFbxDeformer::eSKIN);
	KFbxSkin::ESkinningType lSkinningType = lSkinDeformer->GetSkinningType();

	if(lSkinningType == KFbxSkin::eLINEAR || lSkinningType == KFbxSkin::eRIGID)
	{
		ComputeLinearDeformation(pGlobalPosition, pMesh, pTime, pVertexArray, pPose);
	}
	else if(lSkinningType == KFbxSkin::eDUALQUATERNION)
	{
		ComputeDualQuaternionDeformation(pGlobalPosition, pMesh, pTime, pVertexArray, pPose);
	}
	else if(lSkinningType == KFbxSkin::eBLEND)
	{
		int lVertexCount = pMesh->GetControlPointsCount();

		KFbxVector4* lVertexArrayLinear = new KFbxVector4[lVertexCount];
		memcpy(lVertexArrayLinear, pMesh->GetControlPoints(), lVertexCount * sizeof(KFbxVector4));

		KFbxVector4* lVertexArrayDQ = new KFbxVector4[lVertexCount];
		memcpy(lVertexArrayDQ, pMesh->GetControlPoints(), lVertexCount * sizeof(KFbxVector4));

		ComputeLinearDeformation(pGlobalPosition, pMesh, pTime, lVertexArrayLinear, pPose);
		ComputeDualQuaternionDeformation(pGlobalPosition, pMesh, pTime, lVertexArrayDQ, pPose);

		// To blend the skinning according to the blend weights
		// Final vertex = DQSVertex * blend weight + LinearVertex * (1- blend weight)
		// DQSVertex: vertex that is deformed by dual quaternion skinning method;
		// LinearVertex: vertex that is deformed by classic linear skinning method;
		int lBlendWeightsCount = lSkinDeformer->GetControlPointIndicesCount();
		for(int lBWIndex = 0; lBWIndex<lBlendWeightsCount; ++lBWIndex)
		{
			double lBlendWeight = lSkinDeformer->GetControlPointBlendWeights()[lBWIndex];
			pVertexArray[lBWIndex] = lVertexArrayDQ[lBWIndex] * lBlendWeight + lVertexArrayLinear[lBWIndex] * (1 - lBlendWeight);
		}
	}
}


void ReadVertexCacheData(KFbxMesh* pMesh, 
                         KTime& pTime, 
                         KFbxVector4* pVertexArray)
{
    KFbxVertexCacheDeformer* lDeformer     = static_cast<KFbxVertexCacheDeformer*>(pMesh->GetDeformer(0, KFbxDeformer::eVERTEX_CACHE));
    KFbxCache*               lCache        = lDeformer->GetCache();
    int                      lChannelIndex = -1;
    unsigned int             lVertexCount  = (unsigned int)pMesh->GetControlPointsCount();
    bool                     lReadSucceed  = false;
    double*                  lReadBuf      = new double[3*lVertexCount];

    if (lCache->GetCacheFileFormat() == KFbxCache::eMC)
    {
        if ((lChannelIndex = lCache->GetChannelIndex(lDeformer->GetCacheChannel())) > -1)
        {
            lReadSucceed = lCache->Read(lChannelIndex, pTime, lReadBuf, lVertexCount);
        }
    }
    else // ePC2
    {
        lReadSucceed = lCache->Read((unsigned int)pTime.GetFrame(true), lReadBuf, lVertexCount);
    }

    if (lReadSucceed)
    {
        unsigned int lReadBufIndex = 0;

        while (lReadBufIndex < 3*lVertexCount)
        {
            // In statements like "pVertexArray[lReadBufIndex/3].SetAt(2, lReadBuf[lReadBufIndex++])", 
            // on Mac platform, "lReadBufIndex++" is evaluated before "lReadBufIndex/3". 
            // So separate them.
            pVertexArray[lReadBufIndex/3].SetAt(0, lReadBuf[lReadBufIndex]); lReadBufIndex++;
            pVertexArray[lReadBufIndex/3].SetAt(1, lReadBuf[lReadBufIndex]); lReadBufIndex++;
            pVertexArray[lReadBufIndex/3].SetAt(2, lReadBuf[lReadBufIndex]); lReadBufIndex++;
        }
    }

    delete [] lReadBuf;
}


// Draw an oriented camera box where the node is located.
void DrawCamera(KFbxNode* pNode, 
                KTime& pTime, 
                KFbxAnimLayer* pAnimLayer,
                KFbxXMatrix& pGlobalPosition)
{
    KFbxXMatrix lCameraGlobalPosition;
    KFbxVector4 lCameraPosition, lCameraDefaultDirection, lCameraInterestPosition;

    lCameraPosition = pGlobalPosition.GetT();

    // By default, FBX cameras point towards the X positive axis.
    KFbxVector4 lXPositiveAxis(1.0, 0.0, 0.0);
    lCameraDefaultDirection = lCameraPosition + lXPositiveAxis;

    lCameraGlobalPosition = pGlobalPosition;

    // If the camera is linked to an interest, get the interest position.
    if (pNode->GetTarget())
    {
        lCameraInterestPosition = GetGlobalPosition(pNode->GetTarget(), pTime).GetT();

        // Compute the required rotation to make the camera point to it's interest.
        KFbxVector4 lCameraDirection;
        KFbxVector4::AxisAlignmentInEulerAngle(lCameraPosition, 
            lCameraDefaultDirection, 
            lCameraInterestPosition, 
            lCameraDirection);

        // Must override the camera rotation 
        // to make it point to it's interest.
        lCameraGlobalPosition.SetR(lCameraDirection);
    }

    // Get the camera roll.
    KFbxCamera* cam = pNode->GetCamera();
    double lRoll = 0;

    if (cam)
    {
        lRoll = cam->Roll.Get();
        KFbxAnimCurve* fc = cam->Roll.GetCurve<KFbxAnimCurve>(pAnimLayer);
        if (fc) fc->Evaluate(pTime);
    }
    GlDrawCamera(lCameraGlobalPosition, lRoll);
}


// Draw a colored sphere or cone where the node is located.
void DrawLight(const KFbxNode* pNode, const KTime& pTime, const KFbxXMatrix& pGlobalPosition)
{
    const KFbxLight* lLight = pNode->GetLight();
    if (!lLight)
        return;

    // Must rotate the light's global position because 
    // FBX lights point towards the Y negative axis.
    KFbxXMatrix lLightRotation;
    const KFbxVector4 lYNegativeAxis(-90.0, 0.0, 0.0);
    lLightRotation.SetR(lYNegativeAxis);
    const KFbxXMatrix lLightGlobalPosition = pGlobalPosition * lLightRotation;

    glPushMatrix();
    glMultMatrixd((const double*)lLightGlobalPosition);

    const LightCache * lLightCache = static_cast<const LightCache *>(lLight->GetUserDataPtr());
    if (lLightCache)
    {
        lLightCache->SetLight(pTime);
    }

    glPopMatrix();
}


// Draw a cross hair where the node is located.
void DrawNull(KFbxXMatrix& pGlobalPosition)
{
    GlDrawCrossHair(pGlobalPosition);
}


// Scale all the elements of a matrix.
void MatrixScale(KFbxXMatrix& pMatrix, double pValue)
{
    int i,j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pMatrix[i][j] *= pValue;
        }
    }
}


// Add a value to all the elements in the diagonal of the matrix.
void MatrixAddToDiagonal(KFbxXMatrix& pMatrix, double pValue)
{
    pMatrix[0][0] += pValue;
    pMatrix[1][1] += pValue;
    pMatrix[2][2] += pValue;
    pMatrix[3][3] += pValue;
}


// Sum two matrices element by element.
void MatrixAdd(KFbxXMatrix& pDstMatrix, KFbxXMatrix& pSrcMatrix)
{
    int i,j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pDstMatrix[i][j] += pSrcMatrix[i][j];
        }
    }
}
