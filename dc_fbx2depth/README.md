------------------------------
Depth Camera I/O interface
Xiaolong ZHU@HKU
lucienzhu@gmail.com
------------------------------

1. Prerequisites
====================================

1.1 Dependencies
------------------
	* FBX SDK for VS2010
	* GLUT and GLEW

	1. http://usa.autodesk.com/adsk/servlet/index?siteID=123112&id=7478532
	2. see the folder ./GL, ./glutx86, ./glutx64

1.2 Settings
--------------------
	* C/C++ Additional Include Path: 
		* C:\Program Files\Autodesk\FBX\FbxSdk\2012.1\include;
		* $(QTDIR)\include;
		* .\GeneratedFiles\$(Configuration);
		* $(QTDIR)\include\qtmain;
		* $(QTDIR)\include\QtCore;
		* $(QTDIR)\include\QtOpenGL;
		* $(QTDIR)\include\QtGui;
		* %(AdditionalIncludeDirectories)
	* Linker Addtional Include Directories:
		* C:\Program Files\Autodesk\FBX\FbxSdk\2012.1\lib\vs2010\x86;
		* $(QTDIR)\lib;
		* .\glutx86;
		* %(AdditionalLibraryDirectories)
	* Linker Input:
		* glew32.lib;fbxsdk-2012.1-mdd.lib;wininet.lib;qtmaind.lib;QtCored4.lib;QtOpenGLd4.lib;opengl32.lib;glu32.lib;QtGuid4.lib;%(AdditionalDependencies)

2. Usage
====================================
Render FBX files into scene, and obtain the depth map and label in certain ROI.

An command line file should also be provided.

3. Empirical Notes
====================================