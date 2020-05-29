#pragma once
#include <windowsx.h>
#include "framework.h"
#include "VideoProcDemo_OpenCV.h"
#include "shobjidl_core.h"
#include "opencv2/opencv.hpp" 
#include <vector>

enum PAINT_TYPE{
	TYPE_LINE,
	TYPE_RECT,
	TYPE_ELLIPSE,
	TYPE_NONE
};

class PaintNode
{
public:
	POINT ptBegin;
	POINT ptEnd;
	PAINT_TYPE ptType;
	PaintNode();
	PaintNode(POINT b, POINT e, PAINT_TYPE t);
	~PaintNode();
};

class PaintData
{
private:
	std::vector<PaintNode> vecNode;
public:
	PaintData();
	~PaintData();
	bool pushNode(PaintNode nd);
	bool popNode(PaintNode nd);
	bool addNode(PaintNode nd, int index,int num);
	PaintNode getNode(int index);
	int getSize();
	void clear();

};

