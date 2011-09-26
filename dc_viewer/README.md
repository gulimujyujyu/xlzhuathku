	*********************************
	Depth Camera I/O interface
	Xiaolong ZHU@HKU
	lucienzhu@gmail.com
	*********************************

#1. Prerequisites

##1.1 Dependencies

*. OpenCV 2.1 for VS2010

1. See [OpenCV for VS2010](http://opencv.willowgarage.com/wiki/VisualC%2B%2B_VS2008)

##1.2 Naming Convention

The depth image, color image, video data and bounding box should follow certain conventions

##1.3 Settings

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

#2. Usage

Viewer of depth and color images

#3. Empirical Notes

#4. Data

dc_viewer now support viewing Real ROI Data

## 4.1 Single Frame pair (Color, Depth)

###For ROI image

It is the hand image pair, which contains,
	
	{datetime}_{ub_lb}capture.png. Whole Data Image Pair(Color,Depth)
	{datetime}_{ub_lb}image.png. Color Image
	{datetime}_{ub_lb}depth.png. Depth Image
	{datetime}_{ub_lb}detail.box. With 7 parameters
		> single image width
		> rect.x
		> rect.y
		> rect.width
		> rect.height
		> depth.low
		> depth.high
	{datetime} := {yyyy_dd_MM_hh_mm_ss_zzz}
	{ub_lb} := {int_int}

###For Normal Image

It contains,
	
	{datetime}_{ub_lb}capture.png. Whole Data Image Pair(Color,Depth)

##4.2 Video pair (Color, Depth)

###For ROI Video

It only contains image data in a rectangle,

	{datetime}_{ub_lb}roicapture.avi. Rect Image Pair(Color,Depth)
	{datetime}_{ub_lb}detail.box. With 7 parameters
		> single image width
		> rect.x
		> rect.y
		> rect.width
		> rect.height
		> depth.low
		> depth.high

###For Normal Video

It only contains image data in a rectangle,

	{datetime}_{ub_lb}capture.avi. Rect Image Pair(Color,Depth)
