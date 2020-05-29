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
//PlayState playState = PlayState::stopped;       // 播放状态     


class VideoManager
{
public:
	// 成员
	cv::VideoCapture vidCap;						// 视频1的读取器
	WCHAR vidName[1024];                   // 视频的文件路径和文件名
	PlayState vidState;

	// 方法
	VideoManager();
	~VideoManager();


};

