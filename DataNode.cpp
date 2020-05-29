#include "DataNode.h"

DataNode::DataNode()
{
	this->vidEffect = VideoEffect::no;
}

DataNode::~DataNode()
{
	delete this->bmi;
}



void DataNode::setBeginSize(int nw, int nh)
{
	this->nBeginWidth = nw;
	this->nBeginHeight = nh;
}

void DataNode::setOutputSize(int nw,int nh)
{
	this->outputWidth = nw;
	this->outputHeight = nh;
}

void DataNode::setBMI()
{

	int pixelBytes = this->img.channels() * (this->img.depth() + 1); // 计算一个像素多少个字节
	this->bmi = (LPBITMAPINFO) new char[sizeof(BITMAPINFO) + 4 * (1 << 8)];
	this->bmi->bmiHeader.biBitCount = 8 * pixelBytes;
	this->bmi->bmiHeader.biWidth = this->img.cols;
	this->bmi->bmiHeader.biHeight = -this->img.rows;
	this->bmi->bmiHeader.biPlanes = 1;
	this->bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	this->bmi->bmiHeader.biCompression = BI_RGB;
	this->bmi->bmiHeader.biClrImportant = 0;
	this->bmi->bmiHeader.biClrUsed = 0;
	this->bmi->bmiHeader.biSizeImage = 0;
	this->bmi->bmiHeader.biXPelsPerMeter = 0;
	this->bmi->bmiHeader.biYPelsPerMeter = 0;

}

void DataNode::changeBMI()
{
	this->bmi->bmiHeader.biWidth = this->img.cols;
	this->bmi->bmiHeader.biHeight = -this->img.rows;
}

void DataNode::deleteBMI()
{
	delete this->bmi;
}


void DataNode::imageResize() {
	cv::Mat dst;
	cv::resize(this->img, dst, cv::Size(this->outputWidth, this->outputHeight));
	this->img = dst;
}

void DataNode::imageConvert()
{
	switch (this->img.channels())
	{
	case 1:
		cv::cvtColor(this->img, this->img, cv::COLOR_GRAY2BGR); // GRAY单通道
		break;
	case 3:
		cv::cvtColor(this->img, this->img, cv::COLOR_BGR2BGRA);  // BGR三通道
		break;
	default:
		break;
	}
}
