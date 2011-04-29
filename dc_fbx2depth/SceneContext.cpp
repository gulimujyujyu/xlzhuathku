/****************************************************************************************

Copyright (C) 2011 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

#include "SceneContext.h"

#include "SceneCache.h"
#include "SetCamera.h"
#include "DrawScene.h"
#include "DrawText.h"
#include "targa.h"
#include "Common.h"
#include <QDateTime>
#include <QImage>

namespace
{
    // Default file of ViewScene example
    const char * SAMPLE_FILENAME = "humanoid.fbx";
	const char * SAMPLE_SAVEPATH = "";

    // Button and action definition
    const int LEFT_BUTTON = 0;
    const int MIDDLE_BUTTON = 1;
    const int RIGHT_BUTTON = 2;

    const int BUTTON_DOWN = 0;
    const int BUTTON_UP = 1;

    // Find all the cameras under this node recursively.
    void FillCameraArrayRecursive(KFbxNode* pNode, KArrayTemplate<KFbxNode*>& pCameraArray)
    {
        if (pNode)
        {
            if (pNode->GetNodeAttribute())
            {
                if (pNode->GetNodeAttribute()->GetAttributeType() == KFbxNodeAttribute::eCAMERA)
                {
					//double a = pNode->GetCamera()->GetNearPlane();
                    pCameraArray.Add(pNode);
                }
            }

            const int lCount = pNode->GetChildCount();
            for (int i = 0; i < lCount; i++)
            {
                FillCameraArrayRecursive(pNode->GetChild(i), pCameraArray);
            }
        }
    }

    // Find all the cameras in this scene.
    void FillCameraArray(KFbxScene* pScene, KArrayTemplate<KFbxNode*>& pCameraArray)
    {
        pCameraArray.Clear();

        FillCameraArrayRecursive(pScene->GetRootNode(), pCameraArray);
    }

    // Find all poses in this scene.
    void FillPoseArray(KFbxScene* pScene, KArrayTemplate<KFbxPose*>& pPoseArray)
    {
        const int lPoseCount = pScene->GetPoseCount();

        for (int i=0; i < lPoseCount; ++i)
        {
            pPoseArray.Add(pScene->GetPose(i));
        }
    }

    void PreparePointCacheData(KFbxScene* pScene)
    {
        // This function show how to cycle through scene elements in a linear way.
        const int lNodeCount = KFbxGetSrcCount<KFbxNode>(pScene);

        for (int lIndex=0; lIndex<lNodeCount; lIndex++)
        {
            KFbxNode* lNode = KFbxGetSrc<KFbxNode>(pScene, lIndex);

            if (lNode->GetGeometry()) 
            {
                int i, lVertexCacheDeformerCount = lNode->GetGeometry()->GetDeformerCount(KFbxDeformer::eVERTEX_CACHE);

                // There should be a maximum of 1 Vertex Cache Deformer for the moment
                lVertexCacheDeformerCount = lVertexCacheDeformerCount > 0 ? 1 : 0;

                for (i=0; i<lVertexCacheDeformerCount; ++i )
                {
                    // Get the Point Cache object
                    KFbxVertexCacheDeformer* lDeformer = static_cast<KFbxVertexCacheDeformer*>(lNode->GetGeometry()->GetDeformer(i, KFbxDeformer::eVERTEX_CACHE));
                    if( !lDeformer ) continue;
                    KFbxCache* lCache = lDeformer->GetCache();
                    if( !lCache ) continue;

                    // Process the point cache data only if the constraint is active
                    if (lDeformer->IsActive())
                    {
                        if (lCache->GetCacheFileFormat() == KFbxCache::ePC2)
                        {
                            // This code show how to convert from PC2 to MC point cache format
                            // turn it on if you need it.
#if 0 
                            if (!lCache->ConvertFromPC2ToMC(KFbxCache::eMC_ONE_FILE, 
                                KTime::GetFrameRate(pScene->GetGlobalTimeSettings().GetTimeMode())))
                            {
                                // Conversion failed, retrieve the error here
                                KString lTheErrorIs = lCache->GetError().GetLastErrorString();
                            }
#endif
                        }
                        else if (lCache->GetCacheFileFormat() == KFbxCache::eMC)
                        {
                            // This code show how to convert from MC to PC2 point cache format
                            // turn it on if you need it.
                            //#if 0 
                            if (!lCache->ConvertFromMCToPC2(KTime::GetFrameRate(pScene->GetGlobalSettings().GetTimeMode()), 0))
                            {
                                // Conversion failed, retrieve the error here
                                KString lTheErrorIs = lCache->GetError().GetLastErrorString();
                            }
                            //#endif
                        }


                        // Now open the cache file to read from it
                        if (!lCache->OpenFileForRead())
                        {
                            // Cannot open file 
                            KString lTheErrorIs = lCache->GetError().GetLastErrorString();

                            // Set the deformer inactive so we don't play it back
                            lDeformer->SetActive(false);
                        }
                    }
                }
            }
        }
    }

    // Load a texture file (TGA only now) into GPU and return the texture object name
    bool LoadTextureFromFile(const KString & pFilePath, unsigned int & pTextureObject)
    {
        if (pFilePath.Right(3).Upper() == "TGA")
        {
            tga_image lTGAImage;

            if (tga_read(&lTGAImage, pFilePath.Buffer()) == TGA_NOERR)
            {
                // Make sure the image is left to right
                if (tga_is_right_to_left(&lTGAImage))
                    tga_flip_horiz(&lTGAImage);

                // Make sure the image is bottom to top
                if (tga_is_top_to_bottom(&lTGAImage))
                    tga_flip_vert(&lTGAImage);

                // Make the image BGR 24
                tga_convert_depth(&lTGAImage, 24);

                // Transfer the texture date into GPU
                glGenTextures(1, &pTextureObject);
                glBindTexture(GL_TEXTURE_2D, pTextureObject);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
                glTexImage2D(GL_TEXTURE_2D, 0, 3, lTGAImage.width, lTGAImage.height, 0, GL_BGR,
                    GL_UNSIGNED_BYTE, lTGAImage.image_data);
                glBindTexture(GL_TEXTURE_2D, 0);

                tga_free_buffers(&lTGAImage);

                return true;
            }
        }

        return false;
    }

    // Triangulate all NURBS, patch and mesh under this node recursively.
    void TriangulateRecursive(KFbxNode* pNode)
    {
        KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

        if (lNodeAttribute)
        {
            if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eMESH ||
                lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eNURB ||
                lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eNURBS_SURFACE ||
                lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::ePATCH)
            {
                KFbxGeometryConverter lConverter(pNode->GetFbxSdkManager());
                lConverter.TriangulateInPlace(pNode);
            }
        }

        const int lChildCount = pNode->GetChildCount();
        for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
        {
            TriangulateRecursive(pNode->GetChild(lChildIndex));
        }
    }

    // Bake node attributes and materials under this node recursively.
    // Currently only mesh, light and material.
    void LoadCacheRecursive(KFbxNode * pNode, KFbxAnimLayer * pAnimLayer, bool pSupportVBO)
    {
        // Bake material and hook as user data.
        const int lMaterialCount = pNode->GetMaterialCount();
        for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex)
        {
            KFbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lMaterialIndex);
            if (lMaterial && !lMaterial->GetUserDataPtr())
            {
                KScopedPtr<MaterialCache> lMaterialCache(new MaterialCache);
                if (lMaterialCache->Initialize(lMaterial))
                {
                    lMaterial->SetUserDataPtr(lMaterialCache.Release());
                }
            }
        }

        KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
        if (lNodeAttribute)
        {
            // Bake mesh as VBO(vertex buffer object) into GPU.
            if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eMESH)
            {
                KFbxMesh * lMesh = pNode->GetMesh();
                if (pSupportVBO && lMesh && !lMesh->GetUserDataPtr())
                {
                    KScopedPtr<VBOMesh> lMeshCache(new VBOMesh);
                    if (lMeshCache->Initialize(lMesh))
                    {
                        lMesh->SetUserDataPtr(lMeshCache.Release());
                    }
                }
            }
            // Bake light properties.
            else if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eLIGHT)
            {
                KFbxLight * lLight = pNode->GetLight();
                if (lLight && !lLight->GetUserDataPtr())
                {
                    KScopedPtr<LightCache> lLightCache(new LightCache);
                    if (lLightCache->Initialize(lLight, pAnimLayer))
                    {
                        lLight->SetUserDataPtr(lLightCache.Release());
                    }
                }
            }
        }

        const int lChildCount = pNode->GetChildCount();
        for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
        {
            LoadCacheRecursive(pNode->GetChild(lChildIndex), pAnimLayer, pSupportVBO);
        }
    }

    // Unload the cache and release the memory under this node recursively.
    void UnloadCacheRecursive(KFbxNode * pNode)
    {
        // Unload the material cache
        const int lMaterialCount = pNode->GetMaterialCount();
        for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex)
        {
            KFbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lMaterialIndex);
            if (lMaterial && lMaterial->GetUserDataPtr())
            {
                MaterialCache * lMaterialCache = static_cast<MaterialCache *>(lMaterial->GetUserDataPtr());
                lMaterial->SetUserDataPtr(NULL);
                delete lMaterialCache;
            }
        }

        KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
        if (lNodeAttribute)
        {
            // Unload the mesh cache
            if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eMESH)
            {
                KFbxMesh * lMesh = pNode->GetMesh();
                if (lMesh && lMesh->GetUserDataPtr())
                {
                    VBOMesh * lMeshCache = static_cast<VBOMesh *>(lMesh->GetUserDataPtr());
                    lMesh->SetUserDataPtr(NULL);
                    delete lMeshCache;
                }
            }
            // Unload the light cache
            else if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eLIGHT)
            {
                KFbxLight * lLight = pNode->GetLight();
                if (lLight && lLight->GetUserDataPtr())
                {
                    LightCache * lLightCache = static_cast<LightCache *>(lLight->GetUserDataPtr());
                    lLight->SetUserDataPtr(NULL);
                    delete lLightCache;
                }
            }
        }

        const int lChildCount = pNode->GetChildCount();
        for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
        {
            UnloadCacheRecursive(pNode->GetChild(lChildIndex));
        }
    }

    // Bake node attributes and materials for this scene and load the textures.
    void LoadCacheRecursive(KFbxScene * pScene, KFbxAnimLayer * pAnimLayer, const char * pFbxFileName, bool pSupportVBO)
    {
        // Load the textures into GPU, only for file texture now
        const int lTextureCount = pScene->GetTextureCount();
        for (int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex)
        {
            KFbxTexture * lTexture = pScene->GetTexture(lTextureIndex);
            KFbxFileTexture * lFileTexture = KFbxCast<KFbxFileTexture>(lTexture);
            if (lFileTexture && !lFileTexture->GetUserDataPtr())
            {
                // Try to load the texture from absolute path
                const KString lFileName = lFileTexture->GetFileName();
                
                // Only TGA textures are supported now.
                if (lFileName.Right(3).Upper() != "TGA")
                {
                    printf("Only TGA textures are supported now: %s\n", lFileName.Buffer());
                    continue;
                }

                GLuint lTextureObject = 0;
                bool lStatus = LoadTextureFromFile(lFileName, lTextureObject);

                if (!lStatus)
                {
                    // Loading texture from absolute path failed and try to use relative path.
					const KString lWorkingDir = KFbxExtractDirectory(pFbxFileName);
                    const KString lRelativeFileName = KString(lWorkingDir) + "/" + lFileTexture->GetFileName();
                    lStatus = LoadTextureFromFile(lRelativeFileName, lTextureObject);
                }

                if (!lStatus)
                {
                    printf("Failed to load texture file: %s\n", lFileName.Buffer());
                    continue;
                }

                if (lStatus)
                {
                    GLuint * lTextureName = new GLuint(lTextureObject);
                    lFileTexture->SetUserDataPtr(lTextureName);
                }
            }
        }

        LoadCacheRecursive(pScene->GetRootNode(), pAnimLayer, pSupportVBO);
    }

    // Unload the cache and release the memory fro this scene and release the textures in GPU
    void UnloadCacheRecursive(KFbxScene * pScene)
    {
        const int lTextureCount = pScene->GetTextureCount();
        for (int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex)
        {
            KFbxTexture * lTexture = pScene->GetTexture(lTextureIndex);
            KFbxFileTexture * lFileTexture = KFbxCast<KFbxFileTexture>(lTexture);
            if (lFileTexture && lFileTexture->GetUserDataPtr())
            {
                GLuint * lTextureName = static_cast<GLuint *>(lFileTexture->GetUserDataPtr());
                lFileTexture->SetUserDataPtr(NULL);
                glDeleteTextures(1, lTextureName);
                delete lTextureName;
            }
        }

        UnloadCacheRecursive(pScene->GetRootNode());
    }
}

bool InitializeOpenGL()
{
    // Initialize GLEW.
    GLenum lError = glewInit();
    if (lError != GLEW_OK)
    {
        printf("GLEW Error: %s\n", glewGetErrorString(lError));
        return false;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // OpenGL 1.5 at least.
    if (!GLEW_VERSION_1_5)
    {
        printf("The OpenGL version should be at least 1.5 to display shaded scene!\n");
        return false;
    }

    return true;
}

SceneContext::SceneContext(QString pFileName, QString pSavePath, int pWindowWidth, int pWindowHeight, bool pSupportVBO)
: qFileName(pFileName), qSavePath(pSavePath), mStatus(UNLOADED),
mSdkManager(NULL), mScene(NULL), mImporter(NULL), mCurrentAnimLayer(NULL), mSelectedNode(NULL),
mPoseIndex(-1), mCameraStatus(CAMERA_NOTHING), mPause(false), mShadingMode(SHADING_MODE_SHADED),
mSupportVBO(pSupportVBO), mCameraZoomMode(ZOOM_FOCAL_LENGTH),
mWindowWidth(pWindowWidth), mWindowHeight(pWindowHeight), mDrawText(new DrawText)
{
	mFileNameByte = qFileName.toLatin1();
	mFileName = mFileNameByte.data();
	mSavePathByte = qSavePath.toLatin1();
	mSavePath = mSavePathByte.data();
    if (mFileName == NULL)
        mFileName = SAMPLE_FILENAME;

	if (mSavePath == NULL)
		mSavePath = SAMPLE_SAVEPATH;

   // Create the FBX SDK manager which is the object allocator for almost 
   // all the classes in the SDK and create the scene.
   InitializeSdkObjects(mSdkManager, mScene);

   if (mSdkManager)
   {
       // Create the importer.
       int lFileFormat = -1;
       mImporter = KFbxImporter::Create(mSdkManager,"");
       if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(mFileName, lFileFormat) )
       {
           // Unrecognizable file format. Try to fall back to KFbxImporter::eFBX_BINARY
           lFileFormat = mSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription( "FBX binary (*.fbx)" );;
       }

       // Initialize the importer by providing a filename.
       if(mImporter->Initialize(mFileName, lFileFormat) == true)
       {
           // The file is going to be imported at 
           // the end of the first display callback.
           mWindowMessage = "Importing file ";
           mWindowMessage += mFileName;
           mWindowMessage += "\nPlease wait!";

           // Set scene status flag to ready to load.
           mStatus = MUST_BE_LOADED;
       }
       else
       {
           mWindowMessage = "Unable to open file ";
           mWindowMessage += mFileName;
           mWindowMessage += "\nError reported: ";
           mWindowMessage += mImporter->GetLastErrorString();
           mWindowMessage += "\nEsc to exit";
       }
   }
   else
   {
       mWindowMessage = "Unable to create the FBX SDK manager";
       mWindowMessage += "\nEsc to exit";
   }

   mROI_x = 0;
   mROI_y = 0;
   mROI_w = mWindowWidth;
   mROI_h = mWindowHeight;
}

SceneContext::~SceneContext()
{
    FbxSdkDeleteAndClear(mAnimStackNameArray);

    delete mDrawText;

    // Unload the cache and free the memory
    if (mScene)
    {
        UnloadCacheRecursive(mScene);
    }

    // Delete the FBX SDK manager. All the objects that have been allocated 
    // using the FBX SDK manager and that haven't been explicitly destroyed 
    // are automatically destroyed at the same time.
    if (mSdkManager)
    {
        mSdkManager->Destroy();
        mSdkManager = NULL;
        mScene = NULL;
    }
}

void SceneContext::setROI(int x, int y, int w, int h)
{
	mROI_x = x;
	mROI_y = y;
	mROI_w = w;
	mROI_h = h;
}

bool SceneContext::LoadFile()
{
    bool lResult = false;
    // Make sure that the scene is ready to load.
    if (mStatus == MUST_BE_LOADED)
    {
        if (mImporter->Import(mScene) == true)
        {
            // Set the scene status flag to refresh 
            // the scene in the first timer callback.
            mStatus = MUST_BE_REFRESHED;

			/*
			//BUG IN MY PROGRAM, ALL SHOULD BE IN MILLIMETER!
			//NOTE BY: ZHUXL
            // Convert Axis System to what is used in this example, if needed
            KFbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
            KFbxAxisSystem OurAxisSystem(KFbxAxisSystem::YAxis, KFbxAxisSystem::ParityOdd, KFbxAxisSystem::RightHanded);
            if( SceneAxisSystem != OurAxisSystem )
            {
                OurAxisSystem.ConvertScene(mScene);
            }

            // Convert Unit System to what is used in this example, if needed
            KFbxSystemUnit SceneSystemUnit = mScene->GetGlobalSettings().GetSystemUnit();
            if( SceneSystemUnit.GetScaleFactor() != 1.0 )
            {
                //The unit in this example is centimeter.
                KFbxSystemUnit::cm.ConvertScene( mScene);
            }
			*/

            // Get the list of all the animation stack.
            mScene->FillAnimStackNameArray(mAnimStackNameArray);

            // Get the list of all the cameras in the scene.
            FillCameraArray(mScene, mCameraArray);

            // Convert mesh, NURBS and patch into triangle mesh
            TriangulateRecursive(mScene->GetRootNode());

            // Bake the scene for one frame
            LoadCacheRecursive(mScene, mCurrentAnimLayer, mFileName, mSupportVBO);

            // Convert any .PC2 point cache data into the .MC format for 
            // vertex cache deformer playback.
            PreparePointCacheData(mScene);

            // Get the list of pose in the scene
            FillPoseArray(mScene, mPoseArray);

            // Initialize the window message.
            mWindowMessage = "File ";
            mWindowMessage += mFileName;
            mWindowMessage += "\nClick on the right mouse button to enter menu.";
            mWindowMessage += "\nEsc to exit.";

            // Initialize the frame period.
            mFrameTime.SetTime(0, 0, 0, 1, 0, mScene->GetGlobalSettings().GetTimeMode());

            // Print the keyboard shortcuts.
            printf("Play/Pause Animation: Space Bar.\n");
            printf("Camera Rotate: Left Mouse Button.\n");
            printf("Camera Pan: Left Mouse Button + Middle Mouse Button.\n");
            printf("Camera Zoom: Middle Mouse Button.\n");

            lResult = true;
        }
        else
        {
            // Import failed, set the scene status flag accordingly.
            mStatus = UNLOADED;

            mWindowMessage = "Unable to import file ";
            mWindowMessage += mFileName;
            mWindowMessage += "\nError reported: ";
            mWindowMessage += mImporter->GetLastErrorString();
        }

        // Destroy the importer to release the file.
        mImporter->Destroy();
        mImporter = NULL;
    }

    return lResult;
}

bool SceneContext::SetCurrentAnimStack(int pIndex)
{
    const int lAnimStackCount = mAnimStackNameArray.GetCount();
    if (!lAnimStackCount || pIndex >= lAnimStackCount)
    {
        return false;
    }

    // select the base layer from the animation stack
   KFbxAnimStack * lCurrentAnimationStack = mScene->FindMember(FBX_TYPE(KFbxAnimStack), mAnimStackNameArray[pIndex]->Buffer());
   if (lCurrentAnimationStack == NULL)
   {
       // this is a problem. The anim stack should be found in the scene!
       return false;
   }

   // we assume that the first animation layer connected to the animation stack is the base layer
   // (this is the assumption made in the FBXSDK)
   mCurrentAnimLayer = lCurrentAnimationStack->GetMember(FBX_TYPE(KFbxAnimLayer), 0);
   mScene->GetEvaluator()->SetContext(lCurrentAnimationStack);

   KFbxTakeInfo* lCurrentTakeInfo = mScene->GetTakeInfo(*(mAnimStackNameArray[pIndex]));
   if (lCurrentTakeInfo)
   {
       mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
       mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
   }
   else
   {
       // Take the time line value
       KTimeSpan lTimeLineTimeSpan;
       mScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);

       mStart = lTimeLineTimeSpan.GetStart();
       mStop  = lTimeLineTimeSpan.GetStop();
   }

   mCurrentTime = mStart;

   // Set the scene status flag to refresh 
   // the scene in the next timer callback.
   mStatus = MUST_BE_REFRESHED;

   return true;
}

bool SceneContext::CloneCameraToDefaultCamera(const char * pCameraName)
{
	if (!pCameraName)
	{
		return false;
	}

	KFbxCamera* lRet = NULL;
	KFbxGlobalCameraSettings& lGlobalCameraSettings = mScene->GlobalCameraSettings();
	KFbxNode* lCameraNode = mScene->FindNodeByName( pCameraName);
	if ( lCameraNode) {
		KFbxCamera* lRet = lCameraNode->GetCamera();
		if ( !(lGlobalCameraSettings.CopyProducerCamera(PRODUCER_PERSPECTIVE, lRet))) {
			return false;
		}			
	}

	mStatus = MUST_BE_REFRESHED;
	return true;
}

bool SceneContext::SetCurrentCamera(const char * pCameraName)
{
    if (!pCameraName)
    {
        return false;
    }
	KFbxCamera* lRet = NULL;
	KFbxNode* lCameraNode = mScene->FindNodeByName( pCameraName);
	if( lCameraNode)
	{
		lRet = lCameraNode->GetCamera();
		lRet->ProjectionType.Set(KFbxCamera::ePERSPECTIVE);
	}

    KFbxGlobalSettings& lGlobalCameraSettings = mScene->GetGlobalSettings();
    lGlobalCameraSettings.SetDefaultCamera(pCameraName);

    mStatus = MUST_BE_REFRESHED;
    return true;
}

bool SceneContext::SetCurrentPoseIndex(int pPoseIndex)
{
    mPoseIndex = pPoseIndex;
    mStatus = MUST_BE_REFRESHED;
    return true;
}

void SceneContext::OnTimerClick() const
{
    // Loop in the animation stack if not paused.
    if (mStop > mStart && !mPause)
    {
        // Set the scene status flag to refresh 
        // the scene in the next timer callback.
        mStatus = MUST_BE_REFRESHED;

        mCurrentTime += mFrameTime;

        if (mCurrentTime > mStop)
        {
            mCurrentTime = mStart;
			exit(0);
        }
    }
    // Avoid displaying the same frame on 
    // and on if the animation stack has no length.
    else
    {
        // Set the scene status flag to avoid refreshing 
        // the scene in the next timer callback.
        mStatus = REFRESHED;
    }
}

// Redraw the scene
bool SceneContext::OnDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Test if the scene has been loaded yet.
    if (mStatus != UNLOADED && mStatus != MUST_BE_LOADED)
    {
        glPushAttrib(GL_ENABLE_BIT);
        glPushAttrib(GL_LIGHTING_BIT);
        glEnable(GL_DEPTH_TEST);
        // Draw the front face only, except for the texts and lights.
        glEnable(GL_CULL_FACE);

		////DEBUG
		//
		//KFbxCamera *cam = GetCurrentCamera(mScene);
		//KFbxVector4 p1 = cam->Position.Get();
		//KFbxVector4 p2 = cam->InterestPosition.Get();
		//const char* camName = cam->GetName();

		//KFbxGlobalCameraSettings& lGlobalCameraSettings = mScene->GlobalCameraSettings();
		//KFbxGlobalSettings& lGlobalSettings = mScene->GetGlobalSettings();
		//KString lCurrentCameraName = lGlobalSettings.GetDefaultCamera();
		//if (cam->ProjectionType.Get() == KFbxCamera::ePERSPECTIVE) {
		//	;
		//}
		

        // Set the view to the current camera settings.
        SetCamera(mScene, mCurrentTime, mCurrentAnimLayer, mCameraArray,
            mWindowWidth, mWindowHeight);

        KFbxPose * lPose = NULL;
        if (mPoseIndex != -1)
        {
            lPose = mScene->GetPose(mPoseIndex);
        }

        // If one node is selected, draw it and its children.
        KFbxXMatrix lDummyGlobalPosition;
        
        if (mSelectedNode)
        {
            // Set the lighting before other things.
            InitializeLights(mScene, mCurrentTime, lPose);
            DrawNodeRecursive(mSelectedNode, mCurrentTime, mCurrentAnimLayer, lDummyGlobalPosition, lPose, mShadingMode);
			//SaveDepthMap();
            //DisplayGrid(lDummyGlobalPosition);
        }
        // Otherwise, draw the whole scene.
        else
        {
            InitializeLights(mScene, mCurrentTime, lPose);
            DrawNodeRecursive(mScene->GetRootNode(), mCurrentTime, mCurrentAnimLayer, lDummyGlobalPosition, lPose, mShadingMode);
			//SaveDepthMap();
            //DisplayGrid(lDummyGlobalPosition);
        }

        glPopAttrib();
        glPopAttrib();
    }

    DisplayWindowMessage();

    return true;
}

void SceneContext::OnReshape(int pWidth, int pHeight)
{
    glViewport(0, 0, (GLsizei)pWidth, (GLsizei)pHeight);
    mWindowWidth = pWidth;
    mWindowHeight = pHeight;
}

void SceneContext::OnKeyboard(unsigned char pKey, int pX, int pY)
{
    // Zoom In on '+' or '=' keypad keys
    if (pKey == 43 || pKey == 61)
    {
        KFbxCamera* lCamera = GetCurrentCamera(mScene);
        if(lCamera)
        {
            //double lOriginalAperture = sqrt(lCamera->GetApertureWidth());
            CameraZoom(mScene, 10, mCameraZoomMode);
            mStatus = MUST_BE_REFRESHED;
        }
    }

    // Zoom Out on '-' or '_' keypad keys
    if (pKey == 45 || pKey == 95)
    {
        KFbxCamera* lCamera = GetCurrentCamera(mScene);
        if(lCamera)
        {
            //double lOriginalAperture = sqrt(lCamera->GetApertureWidth()); 
            CameraZoom(mScene, 0 - 10, mCameraZoomMode);
            mStatus = MUST_BE_REFRESHED;
        }
    }

    // 'N' enable/disable normal display
    if (pKey == 'N' || pKey == 'n')
    {
        //gOGLScene->GetShadingManager()->SetDrawNormal(!gOGLScene->GetShadingManager()->IsDrawNormal());
    }

    // Pause and unpause when spacebar is pressed.
    if (pKey == ' ')
    {
        SetPause(!GetPause());
    }

	// 'C' or 'c' to capture current frame
	if (pKey == 'C' || pKey == 'c')
	{
		SaveDepthMap();
	}
}

void SceneContext::OnMouse(int pButton, int pState, int pX, int pY)
{
    // Move the camera (orbit, zoom or pan) with the mouse.
    KFbxCamera* lCamera = GetCurrentCamera(mScene);
    if (lCamera)
    {
        mCamPosition = lCamera->Position.Get();
        mCamCenter = lCamera->InterestPosition.Get();
        mRoll = lCamera->Roll.Get();
    }
    mLastX = pX;
    mLastY = pY;

    switch (pButton)
    {
    case LEFT_BUTTON:
        // ORBIT (or PAN)
        switch (pState)
        {
        case BUTTON_DOWN:
            if (mCameraStatus == CAMERA_ZOOM)
            {
                mCameraStatus = CAMERA_PAN;
            }
            else
            {
                mCameraStatus = CAMERA_ORBIT;
            }
            break;

        default:
            if (mCameraStatus == CAMERA_PAN)
            {
                mCameraStatus = CAMERA_ZOOM;
            }
            else
            {
                mCameraStatus = CAMERA_NOTHING;
            }
            break;
        }
        break;

    case MIDDLE_BUTTON:
        // ZOOM (or PAN)
        switch (pState)
        {
        case BUTTON_DOWN:
            if (mCameraStatus == CAMERA_ORBIT)
            {
                mCameraStatus = CAMERA_PAN;
            }
            else
            {
                mCameraStatus = CAMERA_ZOOM;
            }
            break;
        
        default:
            if (mCameraStatus == CAMERA_PAN)
            {
                mCameraStatus = CAMERA_ORBIT;
            }
            else
            {
                mCameraStatus = CAMERA_NOTHING;
            }
            break;
        }
        break;
    }
}

void SceneContext::OnMouseMotion(int pX, int pY)
{
    int motion;

    switch (mCameraStatus)
    {
    case CAMERA_ORBIT:
        CameraOrbit(mScene, mCamPosition, mRoll, pX-mLastX, mLastY-pY);
        mStatus = MUST_BE_REFRESHED;
        break;

    case CAMERA_ZOOM:
        motion = mLastY-pY;
        CameraZoom(mScene, motion, mCameraZoomMode);
        mLastY = pY;
        mStatus = MUST_BE_REFRESHED;
        break;

    case CAMERA_PAN:
        CameraPan(mScene, mCamPosition, mCamCenter, mRoll, pX-mLastX, mLastY-pY);
        mStatus = MUST_BE_REFRESHED;
        break;
    }
}

void SceneContext::SetSelectedNode(KFbxNode * pSelectedNode)
{
    mSelectedNode = pSelectedNode;
    mStatus = MUST_BE_REFRESHED;
}

void SceneContext::SetShadingMode(ShadingMode pMode)
{
    mShadingMode = pMode;
    mStatus = MUST_BE_REFRESHED;
}

void SceneContext::DisplayWindowMessage()
{
    glColor3f(1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, mWindowWidth, 0, mWindowHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Display message in the left up corner of the window
    const float lX = 5;
    const float lY = static_cast<float>(mWindowHeight) - 20;
    glTranslatef(lX, lY, 0);

    mDrawText->SetPointSize(15.f);
    mDrawText->Display(mWindowMessage.Buffer());

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void SceneContext::CreateSceneCamera()
{
	//Create camera
	KFbxCamera* zxlCamera = KFbxCamera::Create(mScene, "ZXLCamera");
	zxlCamera->SetFormat(KFbxCamera::eNTSC);
	KFbxNode* zxlCameraNode = KFbxNode::Create(mScene, "ZXLCameraNode");
	zxlCameraNode->SetNodeAttribute(zxlCamera);
	KFbxNode* rootNode = mScene->GetRootNode();
	rootNode->AddChild(zxlCameraNode);
	//create maker
	KFbxMarker* zxlMarker = KFbxMarker::Create(mScene, "ZXLMarker");
	KFbxNode* zxlMarkerNode = KFbxNode::Create(mScene, "ZXLMarkerNode");
	zxlMarkerNode->SetNodeAttribute(zxlMarker);
	rootNode->AddChild(zxlMarkerNode);
	//set up node
	zxlMarkerNode->LclTranslation.Set (KFbxVector4(0.0, 0.0, 0.0));
	zxlMarkerNode->LclRotation.Set (KFbxVector4(0.0, 0.0, 0.0));
	zxlMarkerNode->LclScaling.Set (KFbxVector4(1.0, 1.0, 1.0));

	zxlCamera->Position.Set(KFbxVector4(40.0, 40.0, 0.0));
	KFbxVector4 p1 = zxlCamera->Position.Get();
	//point to marker
	zxlCameraNode->SetTarget(zxlMarkerNode);
}

void SceneContext::MoveCameraABit( int x, int y)
{
	//CameraOrbit(mScene, mCamPosition, mRoll, pX-mLastX, mLastY-pY);
	SetCameraPositionByLL(mScene, mCamPosition, x, y);
	mStatus = MUST_BE_REFRESHED;
}

void SceneContext::SaveDepthMap()
{
	//TODO
	int xx, yy;
	float dd, rr, gg, bb;
	float* dbuf = new float[mROI_w*mROI_h];
	float* cbuf = new float[mROI_w*mROI_h*3];
	glReadPixels( mROI_x, mROI_y, mROI_w, mROI_h, GL_DEPTH_COMPONENT, GL_FLOAT, dbuf);
	glReadPixels( mROI_x, mROI_y, mROI_w, mROI_h, GL_RGB, GL_FLOAT, cbuf);
	QImage dimg(mROI_w, mROI_h, QImage::Format_ARGB32);
	QImage cimg(mROI_w, mROI_h, QImage::Format_ARGB32);
	for (yy=0;yy<mROI_h;yy++) {
		for ( xx=0;xx<mROI_w;xx++){
			dd = dbuf[xx+yy*mROI_w];
			rr = cbuf[(xx+yy*mROI_w)*3+0];
			gg = cbuf[(xx+yy*mROI_w)*3+1];
			bb = cbuf[(xx+yy*mROI_w)*3+2];
			dimg.setPixel(xx,mROI_h-1-yy,qRgb(dd*255,dd*255,dd*255));
			cimg.setPixel(xx,mROI_h-1-yy,qRgb(rr*255,gg*255,bb*255));
		}
	}
	QString filename = qSavePath+QDateTime::currentDateTime().toString("yyyy_dd_MM_hh_mm_ss_zzz");
	filename += QString("depth.png");
	dimg.save(filename);
	filename = qSavePath+QDateTime::currentDateTime().toString("yyyy_dd_MM_hh_mm_ss_zzz");
	filename += QString("color.png");
	cimg.save( filename);
	delete dbuf;	
	delete cbuf;
}

void SceneContext::DisplayGrid(const KFbxXMatrix & pTransform)
{
    glPushMatrix();
    glMultMatrixd(pTransform);

    // Draw a grid 500*500
    glColor3f(0.3f, 0.3f, 0.3f);
    glLineWidth(1.0);
    const int hw = 500;
    const int step = 20;
    const int bigstep = 100;
    int i;

    // Draw Grid
    for (i = -hw; i <= hw; i+=step) {

        if (i % bigstep == 0) {
            glLineWidth(2.0);
        } else {
            glLineWidth(1.0);
        }
        glBegin(GL_LINES);
        glVertex3i(i,0,-hw);
        glVertex3i(i,0,hw);
        glEnd();
        glBegin(GL_LINES);
        glVertex3i(-hw,0,i);
        glVertex3i(hw,0,i);
        glEnd();

    }

    // Write some grid info
    const GLfloat zoffset = -2.f;
    const GLfloat xoffset = 1.f;
    mDrawText->SetPointSize(4.f);
    for (i = -hw; i <= hw; i+=bigstep)
    {

        KString scoord;
        int lCount;

        // Don't display origin
        //if (i == 0) continue;
        if (i == 0) {
            scoord = "0";
            lCount = (int)scoord.GetLen();
            glPushMatrix();
            glVertex3f(i+xoffset,0,zoffset);
            glRotatef(-90,1,0,0);
            
            mDrawText->Display(scoord.Buffer());

            glPopMatrix();

            continue;
        }

        // X coordinates
        scoord = "X: ";
        scoord += i;
        lCount = (int)scoord.GetLen();

        glPushMatrix();
        glTranslatef(i+xoffset,0,zoffset);
        glRotatef(-90,1,0,0);
        mDrawText->Display(scoord.Buffer());
        glPopMatrix();

        // Z coordinates
        scoord = "Z: ";
        scoord += i;
        lCount = (int)scoord.GetLen();

        glPushMatrix();
        glTranslatef(xoffset,0,i+zoffset);
        glRotatef(-90,1,0,0);
        mDrawText->Display(scoord.Buffer());
        glPopMatrix();

    }

    glPopMatrix();
}


void SceneContext::SetZoomMode( CameraZoomMode pZoomMode)
{
    if( pZoomMode == ZOOM_POSITION)
    {
        mCameraZoomMode = ZOOM_POSITION;
    }
    mCameraZoomMode =  ZOOM_FOCAL_LENGTH;
}