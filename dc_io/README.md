------------------------------
Depth Camera I/O interface
Xiaolong ZHU@HKU
lucienzhu@gmail.com
------------------------------

1. Prerequisites
====================================

1.1 Dependencies
------------------
	*. OpenNI + NITE
	*. Qt4.7 for vs2010
	*. Qt4.7 add-on for vs2010
	*. OpenCV 2.1 for vs2010
	*. XVid for windows

	1. See [OpenNI](http://www.openni.org)
	2. See [SensorKinect Driver](https://github.com/avin2/SensorKinect)
	3. See [Install Kinect](http://www.studentguru.gr/blogs/vangos/archive/2011/01/20/how-to-successfully-install-kinect-windows-openni-nite.aspx)
	4. See [OpenCV for 2010](http://opencv.willowgarage.com/wiki/VisualC%2B%2B_VS2008)
	5. See [XVid Codes](http://www.xvidmovies.com/codec/)

1.2 My Settings
-----------------

###1.2.1 Kinect

	0. Download OpenNI stable release OpenNI-Win32-1.0.0.23
	   Download OpenNI Compliant Hardware Binaries Sensor-Win32-5.0.0.24
	   Download OpenNI Compliant Middleware Binaries NITE-Win32-1.3.0.17
	   Download SampleXML from *Install Kinect* as shown above.
	   Download Stable branches from *Sensor Driver* as shown above.
	1. Disconnect Kinect
	   Install OpenNI
	   Install Sensor
	   Install SensorKinect in *Sensor Driver*/Bin
	2. Connect Kinect
	   Check if it is well recognized.
	3. Install NITE
	4. Copy XML files to corresponding files as told in *Install Kinect*.
	5. Run samples in NITE/Samples/Bin/*.exe
	6. You may modify your own project in the similar way to NITE's samples in VS2008. p.s. VS2010 also work.

### 1.2.2 Video Recording

	0. Download OpenCV 2.1 for VS2010
	1. Download XVid Encoder from *XVid Codes*
	2. Choose XVid Encoder from a prompt during runtime.

### 1.2.3 Project Settings
	* C/C++ Additional Include Path: 
		* .\GeneratedFiles;
		* $(QTDIR)\include;
		* .\GeneratedFiles\$(Configuration);
		* $(QTDIR)\include\qtmain;
		* $(QTDIR)\include\QtCore;
		* $(QTDIR)\include\QtGui;
		* $(QTDIR)\include\QtMultimedia;
		* .\;
		* $(OPEN_NI_INCLUDE);
		* $(QTDIR)\include\QtOpenGL;
		* %(AdditionalIncludeDirectories)
	* Linker Addtional Include Directories:
		* $(QTDIR)\lib;
		* $(OPEN_NI_LIB);
		* .\Libs;
		* %(AdditionalLibraryDirectories)
	* Linker Input:
		* OpenNI.lib;qtmaind.lib;QtCored4.lib;QtGuid4.lib;QtOpenGLd4.lib;cv210.lib;cxcore210.lib;highgui210.lib;%(AdditionalDependencies)

2. Usage
====================================
depth camera I/O codes.


3. Empirical Notes
====================================

3.1. Record Codec
--------------
	* High Accuracy:
		* Intel IYUV Codec	    AAAAA
		* Logitech Video		AAAAA
		* Xvid Codec			AAAAA
		* Microsoft Video 1	    AA

