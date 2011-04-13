/*
	ROS bayer pattern to RGB conversion
	Edited by Benedikt Meiler
	benedikt.meiler@mytum.de

	Edited to be used under Windows to convert bayer pattern received by openNI to an openCV Mat

	Usage:
	#include "openni_image_bayer_grbg.h"

	...

	imageGen.SetPixelFormat(XN_PIXEL_FORMAT_GRAYSCALE_8_BIT ); //imageGen == xn::ImageGenerator
	imageGen.GetMetaData(imageMD); // imageMD == xn::ImageMetaData

	openni_wrapper::ImageBayerGRBG imageBayer(&imageMD, openni_wrapper::ImageBayerGRBG::DebayeringMethod(1));// 1 == edge aware

	cv::Mat colorImage = cv::Mat::zeros (480, 640, CV_8UC3); 

	unsigned char* rgb_buffer = (unsigned char*)(colorImage.data ); 
	imageBayer.fillRGB (colorImage.cols, colorImage.rows, rgb_buffer, colorImage.step); 
	cv::cvtColor(colorImage,colorImage,CV_RGB2BGR);  // don't forget openCV uses BGR color order instead of RGB


*/
/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011 2011 Willow Garage, Inc.
 *    Suat Gedikli <gedikli@willowgarage.com>
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "openni_exception.h"
#include <sstream>

namespace openni_wrapper
{

OpenNIException::OpenNIException (const std::string& function_name, const std::string& file_name, unsigned line_number, const std::string& message) throw ()
: function_name_ (function_name)
, file_name_ (file_name)
, line_number_ (line_number)
, message_ (message)
{
  std::stringstream sstream;
  sstream << function_name_ << " @ " << file_name_ << " @ " << line_number_ << " : " << message_;
  message_long_ = sstream.str();
}

OpenNIException::~OpenNIException () throw ()
{
}

OpenNIException& OpenNIException::operator = (const OpenNIException& exception) throw ()
{
  message_ = exception.message_;
  return *this;
}

const char* OpenNIException::what () const throw ()
{
  return message_long_.c_str();
}

const std::string& OpenNIException::getFunctionName () const throw ()
{
  return function_name_;
}

const std::string& OpenNIException::getFileName () const throw ()
{
  return file_name_;
}

unsigned OpenNIException::getLineNumber () const throw ()
{
  return line_number_;
}

} //namespace openni_camera