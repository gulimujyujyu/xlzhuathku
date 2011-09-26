	*********************************
	Data Acquisition for xlzhuathku
	My MPhil Research in HongKongU
	Xiaolong ZHU@HKU
	lucienzhu@gmail.com
	*********************************

#1. Introduction

##1.1  dc_io

depth camera I/O codes.

##1.2  dc_viewer

viewer of depth and color data

##1.3  xlzhuathku

a playground for the time being.

##1.4  dc_fbx2depth

a command line tool to obtain the depth map from certain ROI.

#2. Prerequisites


##2.1 dc_io

See ./dc_io/README

##2.2 dc_viewer

See ./dc_viewer/README

##2.3 xlzhuathku

See ./xlzhuathku/README

##2.4 dc_fbx2depth

See ./dc_fbx2depth/README

#3. Collabrators
* lucienzhu@github
* gulimujyujyu@github

#4. Data

## 4.1 Synthetic

### 4.1.1 Single Frame pair (Color, Depth)

	{lat_lon}_{datetime}color.png. Color Image
	{lat_lon}_{datetime}depth.png. Depth Image
	{datetime} := {yyyy_dd_MM_hh_mm_ss_zzz}
	{lat_lon} := {cameraLatitute_cameraLongitute}

## 4.2 Real

### 4.2.1 Single Frame pair (Color, Depth)

####For ROI image

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

####For Normal Image

It contains,
	
	{datetime}_{ub_lb}capture.png. Whole Data Image Pair(Color,Depth)

###4.2.2 Video pair (Color, Depth)

####For ROI Video

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

####For Normal Video

It only contains image data in a rectangle,

	{datetime}_{ub_lb}capture.avi. Rect Image Pair(Color,Depth)
