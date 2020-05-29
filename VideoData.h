#pragma once
#include "DataNode.h"

class VideoData
{
public:
	DataNode* vidNode1;
	DataNode* vidNode2;
	// 客户端大小
	int nClientWidth;
	int nClientHeight;
	void setClientSize(int nw, int nh);
	VideoData();
	VideoData(int cw, int ch);
	~VideoData();
};

