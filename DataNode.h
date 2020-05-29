#pragma once
#include <windowsx.h>
#include "framework.h"
#include "VideoProcDemo_OpenCV.h"
#include "shobjidl_core.h"
#include "opencv2/opencv.hpp" 

enum VideoEffect
{
	no, edge
};

class DataNode
{
public: // create from serialization only
	DataNode();
	~DataNode();
	
	//void getBMI();
	//void getIMG();
	void setBeginSize(int nw, int nh);
	void setOutputSize(int nw,int nh);
	void setBMI();
	void changeBMI();
	void deleteBMI();
	void imageResize();
	void imageConvert();
	cv::Mat img;
	//BITMAPINFO bmi;
	BITMAPINFO* bmi;
	//int nchannel;	
	


	// ���ͼ���С
	int outputWidth;
	int outputHeight;
	// �߽�ȡֵ
	int nBeginWidth;
	int nBeginHeight;
	
	// �˾�Ч��
	VideoEffect vidEffect;    // ��Ƶ����Ч��
};

