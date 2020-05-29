#include "VideoData.h"


void VideoData::setClientSize(int nw, int nh)
{
	this->nClientWidth = nw;
	this->nClientHeight = nh;
}

VideoData::VideoData()
{
	this->vidNode1 = new DataNode;
	this->vidNode2 = new DataNode;
	this->nClientWidth = this->nClientHeight = 0;
}

VideoData::VideoData(int cw, int ch) :nClientWidth(cw), nClientHeight(ch) {
	this->vidNode1 = new DataNode;
	this->vidNode2 = new DataNode;
}

VideoData::~VideoData()
{
	delete vidNode1;
	delete vidNode2;
}
