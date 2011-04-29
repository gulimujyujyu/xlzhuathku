#include "fbx2depth.h"

#include <fbxsdk.h>
#include "SceneContext.h"
#include "GL/glut.h"

#define FRAC_PI_180		        .017453292519943295769236907684886127134428718885417
#define FRAC_180_PI		        57.295779513082320876798154814105170332405472466565
#define OPTION2_COPY_CAMERA001_TO_DEFAULT_CAMERA
#define SAVE_DEPTH_MAP

SceneContext* gSceneDrawer;
int camLat, camLon;
const int LatStep = 10;
const int LonStep = 10;

const int DEFAULT_WINDOW_WIDTH = 320;
const int DEFAULT_WINDOW_HEIGHT = 240;

/* Tab character ("\t") counter */
int numTabs = 0; 


/**
 * Print the required number of tabs.
 */
void PrintTabs() {
    for(int i = 0; i < numTabs; i++)
        printf("\t");
}

/**
 * Return a string-based representation based on the attribute type.
 */
KString GetAttributeTypeName(KFbxNodeAttribute::EAttributeType type) {
    switch(type) {
        case KFbxNodeAttribute::eUNIDENTIFIED: return "unidentified";
        case KFbxNodeAttribute::eNULL: return "null";
        case KFbxNodeAttribute::eMARKER: return "marker";
        case KFbxNodeAttribute::eSKELETON: return "skeleton";
        case KFbxNodeAttribute::eMESH: return "mesh";
        case KFbxNodeAttribute::eNURB: return "nurb";
        case KFbxNodeAttribute::ePATCH: return "patch";
        case KFbxNodeAttribute::eCAMERA: return "camera";
        case KFbxNodeAttribute::eCAMERA_STEREO:    return "stereo";
        case KFbxNodeAttribute::eCAMERA_SWITCHER: return "camera switcher";
        case KFbxNodeAttribute::eLIGHT: return "light";
        case KFbxNodeAttribute::eOPTICAL_REFERENCE: return "optical reference";
        case KFbxNodeAttribute::eOPTICAL_MARKER: return "marker";
        case KFbxNodeAttribute::eNURBS_CURVE: return "nurbs curve";
        case KFbxNodeAttribute::eTRIM_NURBS_SURFACE: return "trim nurbs surface";
        case KFbxNodeAttribute::eBOUNDARY: return "boundary";
        case KFbxNodeAttribute::eNURBS_SURFACE: return "nurbs surface";
        case KFbxNodeAttribute::eSHAPE: return "shape";
        case KFbxNodeAttribute::eLODGROUP: return "lodgroup";
        case KFbxNodeAttribute::eSUBDIV: return "subdiv";
        default: return "unknown";
    }
}

/**
 * Print an attribute.
 */
void PrintAttribute(KFbxNodeAttribute* pAttribute) {
    if(!pAttribute) return;
 
    KString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
    KString attrName = pAttribute->GetName();
    PrintTabs();
    // Note: to retrieve the chararcter array of a KString, use its Buffer() method.
    printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
}

/**
 * Print a node, its attributes, and all its children recursively.
 */
void PrintNode(KFbxNode* pNode) {
    PrintTabs();
    const char* nodeName = pNode->GetName();
    fbxDouble3 translation = pNode->LclTranslation.Get();
    fbxDouble3 rotation = pNode->LclRotation.Get();
    fbxDouble3 scaling = pNode->LclScaling.Get();

    // print the contents of the node.
    printf("<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n", 
        nodeName, 
        translation[0], translation[1], translation[2],
        rotation[0], rotation[1], rotation[2],
        scaling[0], scaling[1], scaling[2]
        );
    numTabs++;

    // Print the node's attributes.
    for(int i = 0; i < pNode->GetNodeAttributeCount(); i++)
        PrintAttribute(pNode->GetNodeAttributeByIndex(i));

    // Recursively print the children nodes.
    for(int j = 0; j < pNode->GetChildCount(); j++)
        PrintNode(pNode->GetChild(j));

    numTabs--;
    PrintTabs();
    printf("</node>\n");
}

//Step1: read data
//1.1 load data
void loadData( QString filename, QString savePath)
{
	// Change the following filename to a suitable filename value.
	/*QByteArray fileStr = filename.toLatin1();
	lFilename = fileStr.data();

	QByteArray pathStr = savePath.toLatin1();
	lSavePath = pathStr.data();*/
	//const char* lFilename = "E:\\DATA\\[CDC4CV11]\\ForTestFBX\\zxl.FBX";

	// Initialize OpenGL.
	const bool lSupportVBO = InitializeOpenGL();

	// Initialize the sdk manager. This object handles all our memory management.
	gSceneDrawer = new SceneContext(filename, savePath, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, lSupportVBO);

	camLat = -90;
	camLon = 0;
}

//TODO: 1.2 assign labels
void assignLabels()
{
	//TODO
}

//Step2: rendering
void render(int argc, char *argv[])
{
	// Set exit function to destroy objects created by the FBX SDK.
	atexit(destroyAllStuffs);

	//2.1 initial scene
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT); 
	glutInitWindowPosition(100, 100);
	glutCreateWindow("ViewScene");

	//2.2 begin loop
	glutDisplayFunc(DisplayCallback); 
	glutKeyboardFunc(KeyboardCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	glutMainLoop();
}

bool changeDxDy(int &dx, int &dy)
{
	if (dx>90)	{
		return false;
	} else {
		dy += LonStep;
		if ( dy >= 360 || dy < 0) {
			dy = 0;
			dx += LatStep;
		}
		gSceneDrawer->MoveCameraABit(dx, dy);
		return true;
	}
}

// Refresh the application window.
void DisplayCallback()
{
	gSceneDrawer->OnDisplay();

	glutSwapBuffers();

	// Import the scene if it's ready to load.
	if (gSceneDrawer->GetStatus() == SceneContext::MUST_BE_LOADED)
	{
		// This function is only called in the first display callback
		// to make sure that the application window is opened and a 
		// status message is displayed before.
		gSceneDrawer->LoadFile();
#ifndef OPTION2_COPY_CAMERA001_TO_DEFAULT_CAMERA
		//NOTE BY ZHUXL
		//OPTION 1: USE CAMERA TRAJ
		gSceneDrawer->SetCurrentAnimStack(0); //by default, current animation stack is camera movement.
#else
		//NOTE BY ZHUXL
		//OPTION 2: COPY CAMERA SETTING TO PRODUCER PERSPECTIVE CAMERA
		//gSceneDrawer->CreateSceneCamera();
		//gSceneDrawer->SetCurrentCamera("ZXLCamera");
		//gSceneDrawer->SetCurrentCamera("Camera001");
		gSceneDrawer->SetCurrentCamera("Producer Perspective");
		gSceneDrawer->CloneCameraToDefaultCamera("Camera001");
#endif
		// Call the timer to display the first frame.
		glutTimerFunc((unsigned int)gSceneDrawer->GetFrameTime().GetMilliSeconds(), TimerCallback, 0);
	}
}

// Trigger the display of the current frame.
void TimerCallback(int)
{
	// Ask to display the current frame only if necessary.
	if (gSceneDrawer->GetStatus() == SceneContext::MUST_BE_REFRESHED)
	{
		glutPostRedisplay();
	}	
	gSceneDrawer->OnTimerClick();

#ifdef SAVE_DEPTH_MAP
	gSceneDrawer->SaveDepthMap();
#endif
	

#ifdef OPTION2_COPY_CAMERA001_TO_DEFAULT_CAMERA
	if( !changeDxDy(camLat, camLon)) {
		exit(0);
	}
#endif
	
	// Call the timer to display the next frame.
	glutTimerFunc((unsigned int)gSceneDrawer->GetFrameTime().GetMilliSeconds(), TimerCallback, 0);
}

void MouseCallback(int button, int state, int x, int y)
{
	gSceneDrawer->OnMouse(button, state, x, y);
}

void MotionCallback(int x, int y)
{
	gSceneDrawer->OnMouseMotion(x, y);
}

// Exit the application from the keyboard.
void KeyboardCallback(unsigned char pKey, int pX, int pY)
{
	// Exit on ESC key.
	if (pKey == 27)
	{
		exit(0);
	}

	gSceneDrawer->OnKeyboard(pKey, pX, pY);
}

//Step3: ending works
void destroyAllStuffs()
{
	//TODO
}
