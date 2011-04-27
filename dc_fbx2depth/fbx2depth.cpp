#include "fbx2depth.h"

#include <fbxsdk.h>

//globals
KFbxSdkManager* lSdkManager;
KFbxScene* lScene;

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
void loadData( QString filename)
{
	// Change the following filename to a suitable filename value.
	QByteArray fileStr = filename.toLatin1();
	const char* lFilename = fileStr.data();
	//const char* lFilename = "E:\\DATA\\[CDC4CV11]\\ForTestFBX\\zxl.FBX";

	// Initialize the sdk manager. This object handles all our memory management.
	lSdkManager = KFbxSdkManager::Create();

	// Create the io settings object.
	KFbxIOSettings *ios = KFbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Create an importer using our sdk manager.
	KFbxImporter* lImporter = KFbxImporter::Create(lSdkManager,"");

	// Use the first argument as the filename for the importer.
	if(!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
		printf("Call to KFbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetLastErrorString());
		exit(-1);
	}

	// Create a new scene so it can be populated by the imported file.
	lScene = KFbxScene::Create(lSdkManager,"myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

	// The file has been imported; we can get rid of the importer.
	lImporter->Destroy();

	// Print the nodes of the scene and their attributes recursively.
	// Note that we are not printing the root node, because it should
	// not contain any attributes.
	KFbxNode* lRootNode = lScene->GetRootNode();
	if(lRootNode) {
		for(int i = 0; i < lRootNode->GetChildCount(); i++)
			PrintNode(lRootNode->GetChild(i));
	}
	// Destroy the sdk manager and all other objects it was handling.
	lSdkManager->Destroy();
}

//TODO: 1.2 assign labels
void assignLabels()
{
	//TODO
}

//Step2: rendering
//2.1 set viewport
void setViewPort( int i, int j)
{
	//TODO
}

//2.2 render
void renderHand()
{
	//TODO
}
//2.3 obtain depthmap/labelmap
bool captureDepthAndColor( QString path)
{
	//TODO
	return true;
}

//Step3: ending works
void destroyAllStuffs()
{
	//TODO
}
