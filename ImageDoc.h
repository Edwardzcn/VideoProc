#pragma once
#include <windowsx.h>
#include "framework.h"
#include "VideoProcDemo_OpenCV.h"
#include "shobjidl_core.h"
#include "opencv2/opencv.hpp" 

class ImageDoc
{
public: // create from serialization only
	ImageDoc();
	~ImageDoc();
	//void getBMI();
	//void getIMG();
	void Initial();
	cv::Mat img;
	//BITMAPINFO bmi;
	BITMAPINFO* bmi;
	int nchannel;	
};

