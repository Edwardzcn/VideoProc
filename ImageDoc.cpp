#include "ImageDoc.h"

ImageDoc::ImageDoc()
{
}

ImageDoc::~ImageDoc()
{
	delete this->bmi;
}

void ImageDoc::Initial()
{
	switch (this->img.channels())
	{
	case 1:
		cv::cvtColor(this->img, this->img, cv::COLOR_GRAY2BGR); // GRAY��ͨ��
		break;
	case 3:
		cv::cvtColor(this->img, this->img, cv::COLOR_BGR2BGRA);  // BGR��ͨ��
		break;
	default:
		break;
	}

	int pixelBytes = this->img.channels() * (this->img.depth() + 1); // ����һ�����ض��ٸ��ֽ�
	 // ����bitmapinfo(����ͷ)
	//BITMAPINFO bitInfo;
	//this->bmi.bmiHeader.biBitCount = 8 * pixelBytes;
	//this->bmi.bmiHeader.biWidth = this->img.cols;
	//this->bmi.bmiHeader.biHeight = -this->img.rows;
	//this->bmi.bmiHeader.biPlanes = 1;
	//this->bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	//this->bmi.bmiHeader.biCompression = BI_RGB;
	//this->bmi.bmiHeader.biClrImportant = 0;
	//this->bmi.bmiHeader.biClrUsed = 0;
	//this->bmi.bmiHeader.biSizeImage = 0;
	//this->bmi.bmiHeader.biXPelsPerMeter = 0;
	//this->bmi.bmiHeader.biYPelsPerMeter = 0;
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
