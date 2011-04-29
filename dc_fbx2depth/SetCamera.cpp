/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

/////////////////////////////////////////////////////////////////////////
//
// This file contains functions to:
// 1) find the current camera;
// 2) get the relevant settings of a camera depending on it's projection
//    type and aperture mode;
// 3) compute the orientation of a camera.
//
/////////////////////////////////////////////////////////////////////////

#include <fbxsdk.h>

#include "GetPosition.h"
#include "GlFunctions.h"
#include "SetCamera.h"
#include "SceneContext.h"

#define K_PI_180		        .017453292519943295769236907684886127134428718885417
#define K_180_PI		        57.295779513082320876798154814105170332405472466565

#define HFOV2VFOV(h, ar) (2.0 * atan((ar) * tan( (h * K_PI_180) * 0.5)) * K_180_PI) //ar : aspectY / aspectX
#define VFOV2HFOV(v, ar) (2.0 * atan((ar) * tan( (v * K_PI_180) * 0.5)) * K_180_PI) //ar : aspectX / aspectY

KFbxCamera* GetCurrentCamera(KFbxScene* pScene, 
                             KTime& pTime, 
                             KFbxAnimLayer* pAnimLayer,
                             const KArrayTemplate<KFbxNode*>& pCameraArray);
void GetCameraAnimatedParameters(KFbxNode* pNode, 
                                 KTime& pTime,
                                 KFbxAnimLayer* pAnimLayer);
bool IsProducerCamera(KFbxScene*  pScene, KFbxCamera* pCamera);

static double gsOrthoCameraScale = 178.0; 


// Set the view to the current camera settings.
void SetCamera(KFbxScene* pScene, 
               KTime& pTime, 
               KFbxAnimLayer* pAnimLayer,
               const KArrayTemplate<KFbxNode*>& pCameraArray,
               int pWindowWidth, int pWindowHeight)
{
    // Find the current camera at the given time.
    KFbxCamera* lCamera = GetCurrentCamera(pScene, pTime, pAnimLayer, pCameraArray);
    if( lCamera == NULL)
        return;
    KFbxNode*   lCameraNode = lCamera ? lCamera->GetNode() : NULL;

    // Compute the camera position and direction.
    KFbxVector4 lEye(0,0,1);
    KFbxVector4 lCenter(0,0,0);
    KFbxVector4 lUp(0,1,0);
    KFbxVector4 lForward, lRight;

    if (lCamera)
    {
        lEye = lCamera->Position.Get();
        lUp = lCamera->UpVector.Get();
    }

    if (lCameraNode && lCameraNode->GetTarget())
    {
        lCenter = GetGlobalPosition(lCameraNode->GetTarget(), pTime).GetT();
    }
    else
    {
        if (!lCameraNode || IsProducerCamera(pScene, lCamera))
        {
            if (lCamera)
                lCenter = lCamera->InterestPosition.Get();
        }
        else
        {
            // Get the direction
            KFbxXMatrix lGlobalRotation;
            KFbxVector4 lRotationVector(GetGlobalPosition(lCameraNode, pTime).GetR());
            lGlobalRotation.SetR(lRotationVector);

            // Get the length
            KFbxVector4 lInterestPosition(lCamera->InterestPosition.Get());
            KFbxVector4 lCameraGlobalPosition(GetGlobalPosition(lCameraNode, pTime).GetT());
            double      lLength = (KFbxVector4(lInterestPosition - lCameraGlobalPosition).Length());

            // Set the center.
            // A camera with rotation = {0,0,0} points to the X direction. So create a
            // vector in the X direction, rotate that vector by the global rotation amount
            // and then position the center by scaling and translating the resulting vector
            lRotationVector = KFbxVector4(1.0,0,0);
            lCenter = lGlobalRotation.MultT(lRotationVector);
            lCenter *= lLength;
            lCenter += lEye;

            // Update the default up vector with the camera rotation.
            lRotationVector = KFbxVector4(0,1.0,0);
            lUp = lGlobalRotation.MultT(lRotationVector);
        }
    }

    // Align the up vector.
    lForward = lCenter - lEye;
    lForward.Normalize();
    lRight = lForward.CrossProduct(lUp);
    lRight.Normalize();
    lUp = lRight.CrossProduct(lForward);
    lUp.Normalize();

    // Rotate the up vector with the roll value.
    double lRadians = 0;

    if (lCamera)
        lRadians = lCamera->Roll.Get() * K_PI_180;
    lUp = lUp * cos( lRadians) + lRight * sin(lRadians);


    double lNearPlane = 0.01;
    if (lCamera)
        lNearPlane = lCamera->GetNearPlane();
    double lFarPlane = 4000.0;
    if (lCamera)
        lFarPlane = lCamera->GetFarPlane();

    //Get global scaling.
    KFbxVector4 lCameraScaling = GetGlobalPosition(lCameraNode, pTime).GetS();
    static const int  FORWARD_SCALE = 2;
    static const int  RIGHT_SCALE  = 0;
    static const int UP_SCALE = 1;
    
    //scaling near plane and far plane
    lNearPlane *= lCameraScaling[ FORWARD_SCALE];
    lFarPlane *= lCameraScaling[ FORWARD_SCALE];






    // Get the relevant camera settings for a perspective view.
    if (lCamera && lCamera->ProjectionType.Get() == KFbxCamera::ePERSPECTIVE)
    {
        //get the aspect ratio
        KFbxCamera::ECameraAspectRatioMode lCamAspectRatioMode = lCamera->GetAspectRatioMode();
        double lAspectX = lCamera->AspectWidth.Get();
        double lAspectY = lCamera->AspectHeight.Get();
        double lAspectRatio = 1.333333;
        switch( lCamAspectRatioMode)
        {
        case KFbxCamera::eWINDOW_SIZE:
            lAspectRatio = lAspectX / lAspectY;
            break;
        case KFbxCamera::eFIXED_RATIO:
            lAspectRatio = lAspectX;

            break;
        case KFbxCamera::eFIXED_RESOLUTION:
            lAspectRatio = lAspectX / lAspectY * lCamera->GetPixelRatio();
            break;
        case KFbxCamera::eFIXED_WIDTH:
            lAspectRatio = lCamera->GetPixelRatio() / lAspectY;
            break;
        case KFbxCamera::eFIXED_HEIGHT:
            lAspectRatio = lCamera->GetPixelRatio() * lAspectX;
            break;
        default:
            break;

        }

        //get the aperture ratio
        double lFilmHeight = lCamera->GetApertureHeight();
        double lFilmWidth = lCamera->GetApertureWidth() * lCamera->GetSqueezeRatio();
        //here we use Height : Width
        double lApertureRatio = lFilmHeight / lFilmWidth;


        //change the aspect ratio to Height : Width
        lAspectRatio = 1 / lAspectRatio;
        //revise the aspect ratio and aperture ratio
        KFbxCamera::ECameraGateFit lCameraGateFit = lCamera->GateFit.Get();
        switch( lCameraGateFit)
        {

        case KFbxCamera::eFILL_FIT:
            if( lApertureRatio > lAspectRatio)  // the same as eHORIZONTAL_FIT
            {
                lFilmHeight = lFilmWidth * lAspectRatio;
                lCamera->SetApertureHeight( lFilmHeight);
                lApertureRatio = lFilmHeight / lFilmWidth;
            }
            else if( lApertureRatio < lAspectRatio) //the same as eVERTICAL_FIT
            {
                lFilmWidth = lFilmHeight / lAspectRatio;
                lCamera->SetApertureWidth( lFilmWidth);
                lApertureRatio = lFilmHeight / lFilmWidth;
            }
            break;
        case KFbxCamera::eVERTICAL_FIT:
            lFilmWidth = lFilmHeight / lAspectRatio;
            lCamera->SetApertureWidth( lFilmWidth);
            lApertureRatio = lFilmHeight / lFilmWidth;
            break;
        case KFbxCamera::eHORIZONTAL_FIT:
            lFilmHeight = lFilmWidth * lAspectRatio;
            lCamera->SetApertureHeight( lFilmHeight);
            lApertureRatio = lFilmHeight / lFilmWidth;
            break;
        case KFbxCamera::eSTRETCH_FIT:
            lAspectRatio = lApertureRatio;
            break;
        case KFbxCamera::eOVERSCAN_FIT:
            if( lFilmWidth > lFilmHeight)
            {
                lFilmHeight = lFilmWidth * lAspectRatio;
            }
            else
            {
                lFilmWidth = lFilmHeight / lAspectRatio;
            }
            lApertureRatio = lFilmHeight / lFilmWidth;
            break;
        case KFbxCamera::eNO_FIT:
        default:
            break;
        }
        //change the aspect ratio to Width : Height
        lAspectRatio = 1 / lAspectRatio;

        double lFieldOfViewX = 0.0;
        double lFieldOfViewY=0.0;
        if ( lCamera->GetApertureMode() == KFbxCamera::eVERTICAL)
        {
                lFieldOfViewY = lCamera->FieldOfView.Get();
                lFieldOfViewX = VFOV2HFOV( lFieldOfViewY, 1 / lApertureRatio);
        }
        else if (lCamera->GetApertureMode() == KFbxCamera::eHORIZONTAL)
        {
            lFieldOfViewX = lCamera->FieldOfView.Get(); //get HFOV
            lFieldOfViewY = HFOV2VFOV( lFieldOfViewX, lApertureRatio);
        }
        else if (lCamera->GetApertureMode() == KFbxCamera::eFOCAL_LENGTH)
        {
            lFieldOfViewX = lCamera->ComputeFieldOfView(lCamera->FocalLength.Get());    //get HFOV
            lFieldOfViewY = HFOV2VFOV( lFieldOfViewX, lApertureRatio);
        }
        else if (lCamera->GetApertureMode() == KFbxCamera::eHORIZONTAL_AND_VERTICAL) {
            lFieldOfViewX = lCamera->FieldOfViewX.Get();
            lFieldOfViewY = lCamera->FieldOfViewY.Get();
        }



        double lRealScreenRatio = (double)pWindowWidth / (double)pWindowHeight;
        int  lViewPortPosX = 0, 
            lViewPortPosY = 0, 
            lViewPortSizeX = pWindowWidth, 
            lViewPortSizeY = pWindowHeight;
        //compute the view port
        if( lRealScreenRatio > lAspectRatio)
        {
            lViewPortSizeY = pWindowHeight;
            lViewPortSizeX = (int)( lViewPortSizeY * lAspectRatio);
            lViewPortPosY = 0;
            lViewPortPosX = (int)((pWindowWidth - lViewPortSizeX) * 0.5);
        }
        else
        {
            lViewPortSizeX = pWindowWidth;
            lViewPortSizeY = (int)(lViewPortSizeX / lAspectRatio);
            lViewPortPosX = 0;
            lViewPortPosY = (int)((pWindowHeight - lViewPortSizeY) * 0.5);
        }

        //revise the Perspective since we have film offset
        double lFilmOffsetX = lCamera->FilmOffsetX.Get();
        double lFilmOffsetY = lCamera->FilmOffsetY.Get();
        lFilmOffsetX = 0 - lFilmOffsetX / lFilmWidth * 2.0;
        lFilmOffsetY = 0 - lFilmOffsetY / lFilmHeight * 2.0;

        GlSetCameraPerspective( lFieldOfViewY, lAspectRatio, lNearPlane, lFarPlane, lEye, lCenter, lUp, lFilmOffsetX, lFilmOffsetY);



    //glMatrixMode(GL_PROJECTION);
    //double lTestPerpMatrix[ 16];
    //glGetDoublev( GL_PROJECTION_MATRIX, lTestPerpMatrix);

    //lTestPerpMatrix[ 8] -= lFilmOffsetX;
    //lTestPerpMatrix[ 9] -= lFilmOffsetY;
    //
    //glLoadMatrixd( lTestPerpMatrix);
    //glMatrixMode(GL_MODELVIEW);
        

        glViewport( lViewPortPosX, lViewPortPosY, lViewPortSizeX, lViewPortSizeY);
        

    }
    // Get the relevant camera settings for an orthogonal view.
    else
    {
        double lPixelRatio = 1.0;
        if (lCamera)
            lPixelRatio = lCamera->GetPixelRatio();  

        double lLeftPlane, lRightPlane, lBottomPlane, lTopPlane;

        if(pWindowWidth < pWindowHeight) 
        {   
            lLeftPlane   = -gsOrthoCameraScale * lPixelRatio;
            lRightPlane  =  gsOrthoCameraScale * lPixelRatio;
            lBottomPlane = -gsOrthoCameraScale * pWindowHeight / pWindowWidth;
            lTopPlane    =  gsOrthoCameraScale * pWindowHeight / pWindowWidth;
        } 
        else 
        {
            pWindowWidth *= (int) lPixelRatio;
            lLeftPlane   = -gsOrthoCameraScale * pWindowWidth / pWindowHeight;
            lRightPlane  =  gsOrthoCameraScale * pWindowWidth / pWindowHeight;
            lBottomPlane = -gsOrthoCameraScale;
            lTopPlane    =  gsOrthoCameraScale;
        }

        GlSetCameraOrthogonal(lLeftPlane,
            lRightPlane,
            lBottomPlane,
            lTopPlane,
            lNearPlane,
            lFarPlane,
            lEye,
            lCenter,
            lUp);
    }
}


// Find the current camera at the given time.
KFbxCamera* GetCurrentCamera(KFbxScene* pScene, 
                             KTime& pTime, 
                             KFbxAnimLayer* pAnimLayer,
                             const KArrayTemplate<KFbxNode*>& pCameraArray)
{
    KFbxGlobalSettings& lGlobalSettings = pScene->GetGlobalSettings();
    KFbxGlobalCameraSettings& lGlobalCameraSettings = pScene->GlobalCameraSettings();
    KString lCurrentCameraName = lGlobalSettings.GetDefaultCamera();

    // check if we need to create the Producer cameras!
    if (lGlobalCameraSettings.GetCameraProducerPerspective() == NULL &&
        lGlobalCameraSettings.GetCameraProducerBottom() == NULL &&
        lGlobalCameraSettings.GetCameraProducerTop() == NULL &&
        lGlobalCameraSettings.GetCameraProducerFront() == NULL &&
        lGlobalCameraSettings.GetCameraProducerBack() == NULL &&
        lGlobalCameraSettings.GetCameraProducerRight() == NULL &&
        lGlobalCameraSettings.GetCameraProducerLeft() == NULL)
    {
        lGlobalCameraSettings.CreateProducerCameras();
    }

    if (lCurrentCameraName.Compare(PRODUCER_PERSPECTIVE) == 0)
    {
        return lGlobalCameraSettings.GetCameraProducerPerspective();
    }
    else if (lCurrentCameraName.Compare(PRODUCER_BOTTOM) == 0)
    {
        return lGlobalCameraSettings.GetCameraProducerBottom();
    }
    else if (lCurrentCameraName.Compare(PRODUCER_TOP) == 0)
    {
        return lGlobalCameraSettings.GetCameraProducerTop();
    }
    else if (lCurrentCameraName.Compare(PRODUCER_FRONT) == 0)
    {
        return lGlobalCameraSettings.GetCameraProducerFront();
    }
    else if (lCurrentCameraName.Compare(PRODUCER_BACK) == 0)
    {
        return lGlobalCameraSettings.GetCameraProducerBack();
    }
    else if (lCurrentCameraName.Compare(PRODUCER_RIGHT) == 0)
    {
        return lGlobalCameraSettings.GetCameraProducerRight();
    }
    else if (lCurrentCameraName.Compare(PRODUCER_LEFT) == 0)
    {
        return lGlobalCameraSettings.GetCameraProducerLeft();
    }
    else if (lCurrentCameraName.Compare(CAMERA_SWITCHER) == 0)
    {
        KFbxCameraSwitcher* lCameraSwitcher = pScene->GlobalCameraSettings().GetCameraSwitcher();
		KFbxAnimCurve* lCurve = NULL;
		if (lCameraSwitcher)
		{
			lCurve = lCameraSwitcher->CameraIndex.GetCurve<KFbxAnimCurve>(pAnimLayer);
			KFCurve* lFCurve = (lCurve)?lCurve->GetKFCurve():NULL;

			int lCameraIndex = 0;
			if (lFCurve)
				// Get the index of the camera in the camera array.
				lCameraIndex = (int) lFCurve->Evaluate(pTime) - 1;

			if (lCameraIndex >= 0 && lCameraIndex < pCameraArray.GetCount())
			{
				KFbxNode* lNode = pCameraArray[lCameraIndex];

				// Get the animated parameters of the camera.
				GetCameraAnimatedParameters(lNode, pTime, pAnimLayer);

				return (KFbxCamera*) lNode->GetNodeAttribute();
			}
		}
    }
    else
    {
        int i;
        KFbxNode* lNode = NULL;

        // Find the camera in the camera array.
        for (i = 0; i < pCameraArray.GetCount(); i++)
        {
            if (lCurrentCameraName.Compare(pCameraArray[i]->GetName()) == 0)
            {
                lNode = pCameraArray[i];
                break;
            }
        }

        if (lNode)
        {
            // Get the animated parameters of the camera.
            GetCameraAnimatedParameters(lNode, pTime, pAnimLayer);

            return (KFbxCamera*) lNode->GetNodeAttribute();
        }
    }

    return lGlobalCameraSettings.GetCameraProducerPerspective();
}

// Get the animated parameters of a camera contained in the scene
// and store them in the associated member variables contained in 
// the camera.
void GetCameraAnimatedParameters(KFbxNode* pNode, KTime& pTime, KFbxAnimLayer* pAnimLayer)
{
    KFbxCamera* lCamera = (KFbxCamera*) pNode->GetNodeAttribute();
    lCamera->Position.Set(GetGlobalPosition(pNode, pTime).GetT());

    KFbxAnimCurve* fc = lCamera->Roll.GetCurve<KFbxAnimCurve>(pAnimLayer);
    if (fc)
        lCamera->Roll.Set(fc->Evaluate(pTime));

    KFbxCamera::ECameraApertureMode lCameraApertureMode = lCamera->GetApertureMode();
    if (lCameraApertureMode == KFbxCamera::eHORIZONTAL || 
        lCameraApertureMode == KFbxCamera::eVERTICAL) 
    {
        double lFieldOfView = lCamera->FieldOfView.Get();
        fc = lCamera->FieldOfView.GetCurve<KFbxAnimCurve>(pAnimLayer);
        if (fc)
            lFieldOfView = fc->Evaluate(pTime);

        //update FOV and focal length
        lCamera->FieldOfView.Set( lFieldOfView);
        lCamera->FocalLength.Set( lCamera->ComputeFocalLength( lFieldOfView));
        
    }
    else if ( lCameraApertureMode == KFbxCamera::eHORIZONTAL_AND_VERTICAL)
    {
        double lOldFieldOfViewX = lCamera->FieldOfViewX.Get();
        double lOldFieldOfViewY = lCamera->FieldOfViewY.Get();

        //update FOV
        double lNewFieldOfViewX = lOldFieldOfViewX;
        double lNewFieldOfViewY = lOldFieldOfViewY;
        fc = lCamera->FieldOfViewX.GetCurve<KFbxAnimCurve>(pAnimLayer);
        if (fc)
            lNewFieldOfViewX = fc->Evaluate(pTime);

        fc = lCamera->FieldOfViewY.GetCurve<KFbxAnimCurve>(pAnimLayer);
        if (fc)
            lNewFieldOfViewY = fc->Evaluate(pTime);

        lCamera->FieldOfViewX.Set(lNewFieldOfViewX);
        lCamera->FieldOfViewY.Set(lNewFieldOfViewY);

        //update aspect
        double lUpdatedApertureX = lCamera->GetApertureWidth();
        double lUpdatedApertureY = lCamera->GetApertureHeight();
        lUpdatedApertureX *= tan( lNewFieldOfViewX * 0.5 * K_PI_180) / tan( lOldFieldOfViewX * 0.5 * K_PI_180);
        lUpdatedApertureY *= tan( lNewFieldOfViewY * 0.5 * K_PI_180) / tan( lOldFieldOfViewY * 0.5 * K_PI_180);
        
        lCamera->FilmWidth.Set( lUpdatedApertureX);
        lCamera->FilmHeight.Set( lUpdatedApertureY);
        lCamera->FilmAspectRatio.Set( lUpdatedApertureX / lUpdatedApertureY);


    }
    else if ( lCameraApertureMode == KFbxCamera::eFOCAL_LENGTH)
    {
        double lFocalLength = lCamera->FocalLength.Get();
        fc = lCamera->FocalLength.GetCurve<KFbxAnimCurve>(pAnimLayer);
        if (fc && fc ->Evaluate(pTime))
            lFocalLength = fc->Evaluate( pTime);
            

        //update FOV and focal length
            lCamera->FocalLength.Set(lFocalLength);
        lCamera->FieldOfView.Set( lCamera->ComputeFieldOfView( lFocalLength));
    }
}

bool IsProducerCamera(KFbxScene*  pScene, KFbxCamera* pCamera)
{
    KFbxGlobalCameraSettings& lGlobalCameraSettings = pScene->GlobalCameraSettings();
    if (pCamera == lGlobalCameraSettings.GetCameraProducerPerspective())
        return true;
    if (pCamera == lGlobalCameraSettings.GetCameraProducerTop())
        return true;
    if (pCamera == lGlobalCameraSettings.GetCameraProducerBottom())
        return true;
    if (pCamera == lGlobalCameraSettings.GetCameraProducerFront())
        return true;
    if (pCamera == lGlobalCameraSettings.GetCameraProducerBack())
        return true;
    if (pCamera == lGlobalCameraSettings.GetCameraProducerRight())
        return true;
    if (pCamera == lGlobalCameraSettings.GetCameraProducerLeft())
        return true;

    return false;
}

KFbxCamera* GetCurrentCamera(KFbxScene*  pScene)
{
    KFbxCamera* lRet = NULL;
    KString     lCurrentCameraName;

    KFbxGlobalCameraSettings& lGlobalCameraSettings = pScene->GlobalCameraSettings();
    KFbxGlobalSettings& lGlobalSettings = pScene->GetGlobalSettings();

    lCurrentCameraName = lGlobalSettings.GetDefaultCamera();

    if (lCurrentCameraName == PRODUCER_PERSPECTIVE)
    {
        lRet = lGlobalCameraSettings.GetCameraProducerPerspective();
    }
    else if (lCurrentCameraName == PRODUCER_TOP)
    {
        lRet = lGlobalCameraSettings.GetCameraProducerTop();
    }
    else if (lCurrentCameraName == PRODUCER_BOTTOM)
    {
        lRet = lGlobalCameraSettings.GetCameraProducerBottom();
    }
    else if (lCurrentCameraName == PRODUCER_FRONT)
    {
        lRet = lGlobalCameraSettings.GetCameraProducerFront();
    }
    else if (lCurrentCameraName == PRODUCER_BACK)
    {
        lRet = lGlobalCameraSettings.GetCameraProducerBack();
    }
    else if (lCurrentCameraName == PRODUCER_RIGHT)
    {
        lRet = lGlobalCameraSettings.GetCameraProducerRight();
    }
    else if (lCurrentCameraName == PRODUCER_LEFT)
    {
        lRet = lGlobalCameraSettings.GetCameraProducerLeft();
    }
    else
    {
        KFbxNode* lCameraNode = pScene->FindNodeByName( lCurrentCameraName);
        if( lCameraNode)
        {
            lRet = lCameraNode->GetCamera();
        }
    }
    return lRet;
}


double TransformAperture( double pAperture, double pTransform)
{
    double lTransformAperture = ( pAperture + pTransform);
    if( lTransformAperture < 0.25)
    {
        lTransformAperture = 0.25;
    }
    if( lTransformAperture  > 179.0)
        {
        lTransformAperture = 179.0;
    }
    return lTransformAperture;
            }

void UpdatePerspCameraAttributes( KFbxCamera* pCamera, double pNewApertureW, double pNewApertureH)
            {

    if( pCamera == NULL || IsAnimated( pCamera))
        return;
            // update focal length or field of view.
    double lApertureWidthOrig = pCamera->GetApertureWidth();
    double lApertureHeightOrig = pCamera->GetApertureHeight();

    if (pCamera->GetApertureMode() == KFbxCamera::eFOCAL_LENGTH) {
                // update focal length according to hypothetic new apertures.
        double lFLOrig  = pCamera->FocalLength.Get();
        double lFOVOrig = pCamera->ComputeFieldOfView(lFLOrig); // recompute to be sure.
        // transform aperture width and height temporarily
        pCamera->SetApertureWidth( pNewApertureW );
        pCamera->SetApertureHeight(pNewApertureH );
        double lFLNew  = pCamera->ComputeFocalLength(lFOVOrig);
        double lFOVNew = pCamera->ComputeFieldOfView(lFLNew);
        pCamera->FocalLength.Set(lFLNew);
        pCamera->FieldOfView.Set(lFOVNew);
    } else if (pCamera->GetApertureMode() == KFbxCamera::eVERTICAL
        || pCamera->GetApertureMode() == KFbxCamera::eHORIZONTAL) {
            double lFOVOrig = pCamera->FieldOfView.Get();
            double lFLOrig = pCamera->ComputeFocalLength(lFOVOrig); // recompute to be sure.
            // transform aperture width and height temporarily
            pCamera->SetApertureWidth(pNewApertureW );
            pCamera->SetApertureHeight(pNewApertureH );
            double lFOVNew = pCamera->ComputeFieldOfView(lFLOrig);
            double lFLNew = pCamera->ComputeFocalLength(lFOVNew);
            pCamera->FieldOfView.Set(lFOVNew);
            pCamera->FocalLength.Set(lFLNew);
    } else if (pCamera->GetApertureMode() == KFbxCamera::eHORIZONTAL_AND_VERTICAL) {
        double lFOVOrigX = pCamera->FieldOfViewX.Get();
        double lFOVOrigY = pCamera->FieldOfViewY.Get();
        double lFLOrig = pCamera->ComputeFocalLength(lFOVOrigX); // recompute to be sure.
        // transform aperture width and height temporarily
        pCamera->SetApertureWidth(pNewApertureW );
        pCamera->SetApertureHeight(pNewApertureH );
        double lFOVNewX = pCamera->ComputeFieldOfView(lFLOrig);
        double lFOVNewY = pCamera->ComputeFieldOfView(lFLOrig);
        double lFLNew = pCamera->ComputeFocalLength(lFOVNewX);
        pCamera->FieldOfViewY.Set(lFOVNewX);
        pCamera->FieldOfViewY.Set(lFOVNewY);
        pCamera->FocalLength.Set(lFLNew);
            }
            // reset aperture width and height
    pCamera->SetApertureWidth(lApertureWidthOrig);
    pCamera->SetApertureHeight(lApertureHeightOrig);
}


void CameraZoom(KFbxScene* pScene, int pZoomDepth, int pZoomMode)
{
    KFbxCamera* lCamera = GetCurrentCamera(pScene);
    if( lCamera == NULL)
        return;
    if( pZoomMode == SceneContext::ZOOM_FOCAL_LENGTH)
    {
        KFbxNode* lCameraNode = lCamera->GetNode();
        if (lCamera->ProjectionType.Get() == KFbxCamera::ePERSPECTIVE)
        {
            double lTransform = 0 - pZoomDepth / 400.0;

            double lApertureW = lCamera->GetApertureWidth();
            lApertureW = TransformAperture( lApertureW, lTransform);

            double lApertureH = lCamera->GetApertureHeight();
            lApertureH = TransformAperture( lApertureH, lTransform);

            UpdatePerspCameraAttributes( lCamera, lApertureW, lApertureH);


        }
        else
        {
            if( pZoomDepth > 0)
            gsOrthoCameraScale *= 0.8;
            else
                gsOrthoCameraScale *= 1.25;
        }
    }
    else
    {
        KFbxNode*   lCameraNode = lCamera ? lCamera->GetNode() : NULL;

        // Compute the camera position and direction.
        KFbxVector4 lEye(0,0,1);
        KFbxVector4 lCenter(0,0,0);
        KFbxVector4 lForward(0,0,0);

    if (lCamera)
    {
            lEye = lCamera->Position.Get();
        }

        if (lCameraNode && lCameraNode->GetTarget())
            {
            lCenter = lCameraNode->GetTarget()->LclTranslation.Get();
            lForward = lCenter - lEye;
            }
        else
        {
            if (!lCameraNode || IsProducerCamera(pScene, lCamera))
            {
                if (lCamera)
            {
                    lCenter = lCamera->InterestPosition.Get();
                    lForward = lCenter - lEye;
            }
        }
        else
        {
                // Get the direction
                KFbxXMatrix lGlobalRotation;
                KFbxVector4 lRotationVector( lCameraNode->LclRotation.Get());
                lGlobalRotation.SetR(lRotationVector);

                // Set the center.
                // A camera with rotation = {0,0,0} points to the X direction. So create a
                // vector in the X direction, rotate that vector by the global rotation amount
                // and then position the center by scaling and translating the resulting vector
                lRotationVector = KFbxVector4(1.0,0,0);
                lForward = lGlobalRotation.MultT(lRotationVector);
            }
        }
        lForward.Normalize();
        lEye += lForward * pZoomDepth;
        fbxDouble3 lPosition( lEye.GetAt( 0), lEye.GetAt( 1), lEye.GetAt( 2));
        lCamera->Position.Set( lPosition);
        
    }
}

void CameraOrbit(KFbxScene* pScene, KFbxVector4 lOrigCamPos, double OrigRoll, int dX, int dY)
{
    // Orbit the camera horizontally dX degrees, vertically dY degrees.
    KFbxCamera* lCamera = GetCurrentCamera(pScene);
    if (!lCamera) return;
    KFbxGlobalCameraSettings& lGlobalCameraSettings = pScene->GlobalCameraSettings();
    if (lCamera != lGlobalCameraSettings.GetCameraProducerPerspective()) return;
    if (lCamera->LockMode.Get()) return;
    if (dX == 0 && dY == 0) return;

    KFbxVector4 lRotationVector, lNewPosition, lCurPosition;
    KFbxXMatrix lRotation;
    KFbxVector4 lCenter = lCamera->InterestPosition.Get();

    // current position
    KFbxVector4 lPosition = lCamera->Position.Get();
    lCurPosition = lPosition-lCenter;

    // translate
    lNewPosition = lOrigCamPos-lCenter;

    int rotX;
    if (lNewPosition[2] == 0) {
        rotX = 90;
    } else {
        rotX = (int) (atan((double)lNewPosition[0]/(double)lNewPosition[2]) * K_180_PI);
    }
    bool bRoll = (((int)OrigRoll % 360) != 0);
    if (   (lNewPosition[2] < 0 && !bRoll)
        || (lNewPosition[2] > 0 && bRoll) ) {
            dY = -dY;
    }
    if (bRoll) dX = -dX;

    // Center on the X axis (push)
    lRotationVector[1] = -rotX;
    lRotation.SetR(lRotationVector);
    lNewPosition = lRotation.MultT(lNewPosition);
    // Rotation for the vertical movement: around the X axis
    lRotationVector[1] = 0;
    lRotationVector[0] = dY;
    lRotation.SetR(lRotationVector);
    lNewPosition = lRotation.MultT(lNewPosition);
    // Back from the X axis (pop)
    lRotationVector[0] = 0;
    lRotationVector[1] = rotX;
    lRotation.SetR(lRotationVector);
    lNewPosition = lRotation.MultT(lNewPosition);
    // Rotation for the horizontal movement
    lRotationVector[1] = -dX;
    lRotation.SetR(lRotationVector);
    lNewPosition = lRotation.MultT(lNewPosition);

    // Detect camera flip
    if (   lNewPosition[0]*lCurPosition[0] < 0 
        && lNewPosition[2]*lCurPosition[2] < 0) {
            // flip -> roll 180.
            double lRoll = lCamera->Roll.Get();
            lRoll = 180.0-lRoll;
            lCamera->Roll.Set(lRoll);
    }

    // Back from center
    lNewPosition = lNewPosition + lCenter;

    lCamera->Position.Set(lNewPosition);
}

void CameraPan(KFbxScene* pScene, KFbxVector4 lOrigCamPos, KFbxVector4 lOrigCamCenter, double OrigRoll, int dX, int dY) {
    // Pan the camera horizontally dX degrees, vertically dY degrees.
    KFbxCamera* lCamera = GetCurrentCamera(pScene);
    if (!lCamera) return;
    if (!IsProducerCamera(pScene, lCamera)) return;
    if (lCamera->LockMode.Get()) return;
    if (dX == 0 && dY == 0) return;

    KFbxGlobalCameraSettings& lGlobalCameraSettings = pScene->GlobalCameraSettings();

    KFbxVector4 lRotationXV, lRotationYV, lTranslationV;
    KFbxXMatrix lRotationX, lRotationY, lRotationXInverse, lRotationYInverse, lTranslation;
    KFbxVector4 lNewPosition = lOrigCamPos;
    KFbxVector4 lNewCenter = lOrigCamCenter;

    // Translate the camera in dX and dY according to its point of view.
    if (lCamera == lGlobalCameraSettings.GetCameraProducerTop()) {
        lTranslationV[0] = -dX;
        lTranslationV[1] = 0;
        lTranslationV[2] = dY;
    } else if (lCamera == lGlobalCameraSettings.GetCameraProducerBottom()) {
        lTranslationV[0] = dX;
        lTranslationV[1] = 0;
        lTranslationV[2] = dY;
    } else if (lCamera == lGlobalCameraSettings.GetCameraProducerFront()) {
        lTranslationV[0] = -dX;
        lTranslationV[1] = -dY;
        lTranslationV[2] = 0;
    } else if (lCamera == lGlobalCameraSettings.GetCameraProducerBack()) {
        lTranslationV[0] = dX;
        lTranslationV[1] = -dY;
        lTranslationV[2] = 0;
    } else if (lCamera == lGlobalCameraSettings.GetCameraProducerRight()) {
        lTranslationV[0] = 0;
        lTranslationV[1] = -dY;
        lTranslationV[2] = dX;
    } else if (lCamera == lGlobalCameraSettings.GetCameraProducerLeft()) {
        lTranslationV[0] = 0;
        lTranslationV[1] = -dY;
        lTranslationV[2] = -dX;
    } else {
        // Perspective view. More computation.

        // Adjust displacement when there is roll
        bool bRoll = (((int)OrigRoll % 360) != 0);
        if (bRoll) {
            dX = -dX;
            dY = -dY;
        }

        // Compute angles aY and aZ of the camera with Y and Z axis.
        double aZ, aY;
        /// Vector of distance between camera and center (lookat)
        KFbxVector4 lDist = lNewPosition - lNewCenter;
        // Euclidian distance between camera and lookat
        double dist = (double)(lDist[0]*lDist[0]+lDist[1]*lDist[1]+lDist[2]*lDist[2]);
        // aZ
        if (lDist[2] == 0) {
            aZ = 90.0;
        } else {
            aZ = (atan((double)lDist[0]/(double)lDist[2]) * K_180_PI);
        }
        if (lNewPosition[2] < lNewCenter[2]) aZ += 180;
        // aY
        if (dist > 0.001) {
            aY = (asin(sqrt((double)(lDist[1]*lDist[1])/ dist)) * K_180_PI);
        } else {
            aY = 0;
        }
        if (lNewPosition[1] < lNewCenter[1]) aY = -aY;


        // Basis translation
        lTranslationV[0] = -dX;
        lTranslationV[1] = -dY;
        lTranslationV[2] = 0;

        // Rotation around Y axis
        lRotationYV[0] = 0;
        lRotationYV[1] = -aZ;
        lRotationYV[2] = 0;
        lRotationY.SetR(lRotationYV);
        // Rotation around X axis
        lRotationXV[0] = aY;
        lRotationXV[1] = 0;
        lRotationXV[2] = 0;
        lRotationX.SetR(lRotationXV);

        // Modify translation according to aY and aZ.
        lTranslation.SetT(lTranslationV);
        lRotationYInverse = lRotationY.Inverse();
        lRotationXInverse = lRotationX.Inverse();
        lTranslation = lRotationYInverse * lRotationXInverse * lTranslation * lRotationY * lRotationX;
        lTranslationV = lTranslation.GetT();
    }

    // Translate camera and center according to pan.
    lNewPosition += lTranslationV;
    lNewCenter   += lTranslationV;

    lCamera->Position.Set(lNewPosition);
    lCamera->InterestPosition.Set(lNewCenter);
}

void SetCameraPositionByLL(KFbxScene* pScene, KFbxVector4 lOrigCamPos, int lat, int lon)
{
	//TODO
	// Orbit the camera horizontally dX degrees, vertically dY degrees.
	KFbxCamera* lCamera = GetCurrentCamera(pScene);
	if (!lCamera) return;
	//KFbxGlobalCameraSettings& lGlobalCameraSettings = pScene->GlobalCameraSettings();
	//if (lCamera != lGlobalCameraSettings.GetCameraProducerPerspective()) return;
	if (lCamera->LockMode.Get()) return;

	KFbxVector4 lLookAtPosition = lCamera->InterestPosition.Get();
	KFbxVector4 lCurPosition = lCamera->Position.Get();
	KFbxVector4 dif;
	double newZ, newX, newY;

	dif = lLookAtPosition-lCurPosition;
	double len = dif.Length();

	double latInDegree = double(lat) * K_PI_180;
	double lonInDegree = double(lon) * K_PI_180;
	newZ = len * sin(latInDegree);
	newY = len * cos(latInDegree) * sin(lonInDegree);
	newX = len * cos(latInDegree) * cos(lonInDegree);
	dif.Set(newX, newY, newZ);
	lCurPosition = lLookAtPosition + dif;
	lCamera->Position.Set(lCurPosition);
}