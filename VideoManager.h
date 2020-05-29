#pragma once
#include <windowsx.h>
#include "framework.h"
#include "VideoProcDemo_OpenCV.h"
#include "shobjidl_core.h"
#include "opencv2/opencv.hpp" 

                  


enum PlayState
{
	playing, paused, stopped
};
//PlayState playState = PlayState::stopped;       // ����״̬     


class VideoManager
{
public:
	// ��Ա
	cv::VideoCapture vidCap;						// ��Ƶ1�Ķ�ȡ��
	WCHAR vidName[1024];                   // ��Ƶ���ļ�·�����ļ���
	PlayState vidState;

	// ����
	VideoManager();
	~VideoManager();


};

