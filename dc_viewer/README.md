	*********************************
	Depth Camera I/O interface
	Xiaolong ZHU@HKU
	lucienzhu@gmail.com
	*********************************

1. Prerequisites
===============
1.1 Dependencies
------------------
*. OpenCV 2.1 for VS2010

1. See [OpenCV for VS2010](http://opencv.willowgarage.com/wiki/VisualC%2B%2B_VS2008)

1.2 Naming Convention
------------------
The depth image, color image, video data and bounding box should follow certain conventions

1.3 Settings
------------------
* C/C++ Additional Include Path: 
	* .\GeneratedFiles;
	* $(QTDIR)\include;
	* .\GeneratedFiles\$(Configuration);
	* $(QTDIR)\include\qtmain;
	* $(QTDIR)\include\QtCore;
	* $(QTDIR)\include\QtGui;
	* .\;
	* $(QTDIR)\include\QtOpenGL;
	* %(AdditionalIncludeDirectories)
* Linker Addtional Include Directories:
	* $(QTDIR)\lib;
	* %(AdditionalLibraryDirectories)
* Linker Input:
	* qtmaind.lib;QtCored4.lib;QtGuid4.lib;cv210.lib;cxcore210.lib;highgui210.lib;QtOpenGLd4.lib;opengl32.lib;glu32.lib;%(AdditionalDependencies)

2. Usage
========
Viewer of depth and color images

3. Empirical Notes
==================


