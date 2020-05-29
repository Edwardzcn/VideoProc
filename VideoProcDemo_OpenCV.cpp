// VideoProcDemo_OpenCV.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "VideoProcDemo_OpenCV.h"
#include "shobjidl_core.h"
//#include "DataNode.h"
#include "VideoData.h"
#include "VideoManager.h"
#include "PaintData.h"
#include <windowsx.h>

#include "opencv2/opencv.hpp" 

#define MAX_LOADSTRING 100
#define _DEBUG
#define EPS 1e-5

#ifdef _DEBUG    

#define DP0(fmt) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt));OutputDebugString(sOut);}    
#define DP1(fmt,var) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt),var);OutputDebugString(sOut);}    
#define DP2(fmt,var1,var2) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt),var1,var2);OutputDebugString(sOut);}    
#define DP3(fmt,var1,var2,var3) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt),var1,var2,var3);OutputDebugString(sOut);}    

#endif    

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 鼠标指点
static POINT ptBegin;
static POINT ptEnd;
static PAINT_TYPE ptType = PAINT_TYPE::TYPE_NONE;

//DataNode* imgdoc1, * imgdoc2;
VideoData* coreData;
VideoManager* vidm1, * vidm2;
PaintData* paintData;



// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
bool OpenVideoFile(HWND hWnd, LPWSTR* fn);
std::string WCHAR2String(LPCWSTR pwszSrc);


// 双缓存绘制
void DoubleBufferPaint(HWND hWnd, HDC dc);
void DirectPaint(HWND hWnd, HDC dc);
// 鼠标事件
void CatchLBDown(HWND hWnd, LPARAM lParam);
void CatchLBUp(HWND hWnd, LPARAM lParam);

// 相应函数
void CatchSize(HWND hWnd);
void CatchTimer(HWND hWnd);
void CatchPaint(HWND hWnd);
void CatchDestroy(HWND hWnd);
// 菜单栏
void CatchCommandAbout(HWND hWnd);
void CatchCommandExit(HWND hWnd);
void CatchCommandOpen(HWND hWnd, VideoManager*, DataNode*);
void CatchCommandPlay(HWND hWnd);
void CatchCommandPause(HWND hWnd);
void CatchCommandStop(HWND hWnd);
void CatchCommandLine();
void CatchCommandRect();
void CatchCommandEllipse();
void CatchCommandDrawClear();





int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。
	//img = cv::imread("d://wallpaper.jpg");

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_VIDEOPROCDEMOOPENCV, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	//imgdoc1 = new DataNode();
	//imgdoc2 = new DataNode();
	coreData = new VideoData();
	vidm1 = new VideoManager();
	vidm2 = new VideoManager();
	paintData = new PaintData();
#ifdef _DEBUG
	DP1("paintData size=%d\n", paintData->getSize());
#endif // _DEBUG

	// 执行应用程序初始化:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VIDEOPROCDEMOOPENCV));

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VIDEOPROCDEMOOPENCV));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VIDEOPROCDEMOOPENCV);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetTimer(hWnd, 1, 40, NULL);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//  WM_TIMER    - 定时器消息
//  WM_SIZE		- 窗口大小变化消息


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{


	switch (message)
	{
	case WM_COMMAND:
	{
		// 菜单消息
		// WCHAR* fn = (WCHAR*)vidm1->vidName;
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			CatchCommandAbout(hWnd);
			break;
		case IDM_EXIT:
			CatchCommandExit(hWnd);
			break;
		case IDM_OPEN_VID1:
			CatchCommandOpen(hWnd, vidm1, coreData->vidNode1);
			break;
		case IDM_OPEN_VID2:
			CatchCommandOpen(hWnd, vidm2, coreData->vidNode2);
			break;
			//	视频播放菜单
		case IDM_PLAY_VID:
			CatchCommandPlay(hWnd);
			break;
		case IDM_PAUSE_VID:
			CatchCommandPause(hWnd);
			break;
		case IDM_STOP_VID:
			CatchCommandStop(hWnd);
			break;
			// 视频滤镜菜单
		case IDM_EFFECT_NONE:
			coreData->vidNode1->vidEffect = VideoEffect::no;
			break;
		case IDM_EFFECT_EDGE:
			coreData->vidNode1->vidEffect = VideoEffect::edge;
			break;
		case IDM_DRAW_LINE:
			CatchCommandLine();
			break;
		case IDM_DRAW_ELLIPSE:
			CatchCommandEllipse();
			break;
		case IDM_DRAW_RECT:
			CatchCommandRect();
			break;
		case IDM_DRAW_CLEAR:
			CatchCommandDrawClear();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		// 计时器消息
	}
	case WM_TIMER:
		CatchTimer(hWnd);
		break;
	case WM_PAINT:
		CatchPaint(hWnd);
		break;
	case WM_SIZE:
		CatchSize(hWnd);
		break;
	case WM_DESTROY:
		CatchDestroy(hWnd);
		break;
		// 鼠标单击 绘图相关
	case WM_LBUTTONDOWN:
		CatchLBDown(hWnd, lParam);
		break;
	case WM_LBUTTONUP:
		CatchLBUp(hWnd, lParam);
		break;
		// 鼠标双击 移动相关？
	case WM_LBUTTONDBLCLK:
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void CatchSize(HWND hWnd) {
	//if (vidm1->vidCap.isOpened()) {
	//	coreData->vidNode1->changeBMI();
	//}
	LPRECT rect = new tagRECT;
	// 使用 GetClientRect 而非 GetWindowRect
	GetClientRect(hWnd, rect);
	int nWindowWidth = rect->right - rect->left;
	int nWindowHeight = rect->bottom - rect->top;
	coreData->setClientSize(nWindowWidth, nWindowHeight);
	if (coreData->vidNode1->img.rows > 0) {
		// 倍数放缩
		double fWidthTimes = (double)nWindowWidth / coreData->vidNode1->img.cols;
		double fHeightTimes = (double)nWindowHeight / coreData->vidNode1->img.rows;
		int nBeginWidth, nBeginHeight, nWidth, nHeight;
		if (fWidthTimes > fHeightTimes) {
			// Width加黑边
			nWidth = (int)(fHeightTimes * coreData->vidNode1->img.cols);
			nBeginWidth = (nWindowWidth - nWidth) >> 1;
			nHeight = nWindowHeight;
			nBeginHeight = 0;
		}
		else {
			// Height加黑边
			nHeight = (int)(fWidthTimes * coreData->vidNode1->img.rows);
			nBeginHeight = (nWindowHeight - nHeight) >> 1;
			nWidth = nWindowWidth;
			nBeginWidth = 0;
		}



		coreData->vidNode1->setBeginSize(nBeginWidth, nBeginHeight);
		coreData->vidNode1->setOutputSize(nWidth, nHeight);
		// 连锁绑定 当改变大小的时候
		coreData->vidNode1->imageResize();
		coreData->vidNode1->setBMI();
		//coreData->vidNode1->changeBMI();
		DP0("IMG1 RESIZE\n");
		//InvalidateRect(hWnd, NULL, false);
	}
	if (coreData->vidNode2->img.rows > 0) {
		// 右上角固定倍数缩放
		// 0.25
		double ftimes = 0.25;
		double fWidthTimes = (double)nWindowWidth / coreData->vidNode2->img.cols;
		double fHeightTimes = (double)nWindowHeight / coreData->vidNode2->img.rows;
		int nBeginWidth, nBeginHeight, nWidth, nHeight;
		if (fWidthTimes > fHeightTimes) {
			// Width加黑边
			nWidth = (int)(ftimes * fHeightTimes * coreData->vidNode2->img.cols);
			nHeight = (int)(ftimes * nWindowHeight);
			//nBeginWidth = (nWindowWidth - nWidth) >> 1;
			nBeginWidth = nWindowWidth - nWidth;
			nBeginHeight = 0;
		}
		else {
			// Height加黑边
			nHeight = (int)(ftimes * fWidthTimes * coreData->vidNode2->img.rows);
			nWidth = (int)(ftimes * nWindowWidth);
			//nBeginWidth = 0;
			//nBeginHeight = (nWindowHeight - nHeight) >> 1;
			nBeginWidth = nWindowWidth - nWidth;
			nBeginHeight = 0;
		}

		coreData->vidNode2->setBeginSize(nBeginWidth, nBeginHeight);
		coreData->vidNode2->setOutputSize(nWidth, nHeight);
		// 连锁绑定 当改变大小的时候
		coreData->vidNode2->imageResize();
		coreData->vidNode2->setBMI();
		//coreData->vidNode2->changeBMI();
		DP0("IMG2 RESIZE\n");

#ifdef _DEBUG
		DP2("img2 origin opw=%d oph%d\n", coreData->vidNode2->outputWidth, coreData->vidNode2->outputHeight);
#endif // DEBUG
		//InvalidateRect(hWnd, NULL, false);
	}
}

bool LoadVideoImage(VideoManager* vm, DataNode* imd) {
	if (vm->vidCap.isOpened() && vm->vidState == PLAY_STATE::PLAYING)
	{
		vm->vidCap >> imd->img;
#ifdef _DEBUG
		DP0("Load Image\n")
#endif // _DEBUG

			if (imd->img.empty() == false)
			{

				imd->imageConvert();
				if (imd->vidEffect == VideoEffect::edge)
				{
					cv::Mat edgeY, edgeX;
					cv::Sobel(imd->img, edgeY, CV_8U, 1, 0);
					cv::Sobel(imd->img, edgeX, CV_8U, 0, 1);
					imd->img = edgeX + edgeY;
				}
#ifdef _DEBUG
				DP0("Load Success\n");
#endif // _DEBUG
				return true;
			}
		return false;
	}
	return false;
}

void CatchTimer(HWND hWnd)
{
	// 两个Video Manager独立检测
	bool result1 = LoadVideoImage(vidm1, coreData->vidNode1);
	bool result2 = LoadVideoImage(vidm2, coreData->vidNode2);
	if (result1 || result2) {
		InvalidateRect(hWnd, NULL, false);
	}
}

void CatchPaint(HWND hWnd)
{
	//DP0("WM_PAINT\n");
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	// TODO: 在此处添加使用 hdc 的任何绘图代码...
	// 双缓冲
	DoubleBufferPaint(hWnd, hdc);
	// 非双缓冲
	//DirectPaint(hWnd, hdc);
	EndPaint(hWnd, &ps);
}

void CatchDestroy(HWND hWnd)
{
	PostQuitMessage(0);
}

void DoubleBufferPaint(HWND hWnd, HDC dc) {
	// 创建上下文兼容的dc
	HDC mdc = CreateCompatibleDC(dc);
	// 这里不大懂  2000*1000 是缓存区的分辨率 
	HBITMAP hbitmap = CreateCompatibleBitmap(dc, 2000, 1000);
	SelectObject(mdc, hbitmap);
	// 向dc绘图
	DirectPaint(hWnd, mdc);
	// 整体拷贝至输出
	// bug问题
	StretchBlt(
		dc,
		0, 0, coreData->nClientWidth, coreData->nClientHeight,
		//0, 0, 500, 500,
		mdc,
		0, 0, coreData->nClientWidth, coreData->nClientHeight,
		//0, 0, 500, 500,
		SRCCOPY
	);

	// 恢复内存原始数据
	// 删除资源
	DeleteObject(hbitmap);
	DeleteDC(mdc);
}

void DirectPaint(HWND hWnd, HDC dc) {
	if (coreData->vidNode1->img.rows > 0)
	{
		//// 这里要set不可以用change
		coreData->vidNode1->imageResize();
		coreData->vidNode1->setBMI();
		//coreData->vidNode2->imageResize();
		//coreData->vidNode2->setBMI();

#ifdef _DEBUG
		DP2("img1 opw=%d oph%d\n", coreData->vidNode1->outputWidth, coreData->vidNode1->outputHeight);
#endif // DEBUG


		StretchDIBits(
			dc,
			coreData->vidNode1->nBeginWidth, coreData->vidNode1->nBeginHeight, coreData->vidNode1->outputWidth, coreData->vidNode1->outputHeight,
			//0,0,500,500,
			0, 0, coreData->vidNode1->outputWidth, coreData->vidNode1->outputHeight,
			//0,0,500,500,
			coreData->vidNode1->img.data,
			coreData->vidNode1->bmi,
			DIB_RGB_COLORS,
			SRCCOPY
		);
	}
	if (coreData->vidNode2->img.rows > 0)
	{
		//// 这里要set不可以用change
		//coreData->vidNode1->imageResize();
		//coreData->vidNode1->setBMI();
		coreData->vidNode2->imageResize();
		coreData->vidNode2->setBMI();

#ifdef _DEBUG
		DP2("img2 opw=%d oph%d\n", coreData->vidNode2->outputWidth, coreData->vidNode2->outputHeight);
#endif // DEBUG


		//StretchDIBits(
		//	dc,
		//	coreData->vidNode1->nBeginWidth, coreData->vidNode1->nBeginHeight, coreData->vidNode1->outputWidth, coreData->vidNode1->outputHeight,
		//	//0,0,500,500,
		//	0, 0, coreData->vidNode1->outputWidth, coreData->vidNode1->outputHeight,
		//	//0,0,500,500,
		//	coreData->vidNode1->img.data,
		//	coreData->vidNode1->bmi,
		//	DIB_RGB_COLORS,
		//	SRCCOPY
		//);
		StretchDIBits(
			dc,
			coreData->vidNode2->nBeginWidth, coreData->vidNode2->nBeginHeight, coreData->vidNode2->outputWidth, coreData->vidNode2->outputHeight,
			//coreData->vidNode2->nClientWidth-300,0,300,200,
			0, 0, coreData->vidNode2->outputWidth, coreData->vidNode2->outputHeight,
			//0,0,500,500,
			coreData->vidNode2->img.data,
			coreData->vidNode2->bmi,
			DIB_RGB_COLORS,
			SRCCOPY
		);
	}
	// 绘制PaintData
	for (int i = 0; i < paintData->getSize(); i++) {
		PaintNode tmp = paintData->getNode(i);
		switch (tmp.ptType)
		{
		case PAINT_TYPE::TYPE_LINE:
			MoveToEx(dc, tmp.ptBegin.x, tmp.ptBegin.y, NULL);
			LineTo(dc, tmp.ptEnd.x, tmp.ptEnd.y);
			break;
		case PAINT_TYPE::TYPE_RECT:
			Rectangle(dc, tmp.ptBegin.x, tmp.ptBegin.y, tmp.ptEnd.x, tmp.ptEnd.y);
			break;
		case PAINT_TYPE::TYPE_ELLIPSE:
			Ellipse(dc, tmp.ptBegin.x, tmp.ptBegin.y, tmp.ptEnd.x, tmp.ptEnd.y);
			break;
		default:
			break;
		}


	}
}

void CatchLBDown(HWND hWnd, LPARAM lParam)
{
	ptBegin.x = LOWORD(lParam);
	ptBegin.y = LOWORD(lParam);
}

void CatchLBUp(HWND hWnd, LPARAM lParam)
{
	ptEnd.x = LOWORD(lParam);
	ptEnd.y = LOWORD(lParam);
	if (ptType != PAINT_TYPE::TYPE_NONE) {
		paintData->pushNode(PaintNode(ptBegin, ptEnd, ptType));
		DP3("so add Node beginx=%ld endx=%ld type=%d\n", ptBegin.x, ptEnd.x, ptType);
	}
	// 感觉这里是绘制部分
	//HDC hdc = GetDC(hWnd);
	//switch (ptType)
	//{
	//case PAINT_TYPE::LINE:
	//	MoveToEx(hdc, ptBegin.x, ptBegin.y, NULL);
	//	LineTo(hdc, ptEnd.x, ptEnd.y);
	//	break;
	//case PAINT_TYPE::RECT:
	//	Rectangle(hdc, ptBegin.x, ptBegin.y, ptEnd.x, ptEnd.y);
	//	break;
	//case PAINT_TYPE::ELLUPS:
	//	Ellipse(hdc, ptBegin.x, ptBegin.y, ptEnd.x, ptEnd.y);
	//	break;
	//default:
	//	break;
	//}


}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

bool OpenVideoFile(HWND hWnd, LPWSTR* fn)
{
	IFileDialog* pfd = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pfd));

	DWORD dwFlags;
	hr = pfd->GetOptions(&dwFlags);
	hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);

	COMDLG_FILTERSPEC rgSpec[] =
	{
		{ L"MP4", L"*.mp4" },
		{ L"AVI", L"*.avi" },
		{ L"ALL", L"*.*" },
	};

	HRESULT SetFileTypes(UINT cFileTypes, const COMDLG_FILTERSPEC * rgFilterSpec);
	hr = pfd->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);
	hr = pfd->SetFileTypeIndex(1);
	hr = pfd->Show(hWnd);///显示打开文件对话框

	IShellItem* pShellItem = NULL;
	if (SUCCEEDED(hr))
	{
		hr = pfd->GetResult(&pShellItem);
		hr = pShellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, fn);//获取文件的完整路径

		return true;
	}

	return false;

}

std::string WCHAR2String(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen <= 0)
		return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst)
		return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;

	std::string strTmp(pszDst);
	delete[] pszDst;

	return strTmp;
}

void CatchCommandAbout(HWND hWnd)
{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
}

void CatchCommandExit(HWND hWnd)
{
	DestroyWindow(hWnd);
}

void CatchCommandOpen(HWND hWnd, VideoManager* vm, DataNode* imgd)
{
	WCHAR* fn = (WCHAR*)vm->vidName;
	if (OpenVideoFile(hWnd, &fn))
	{
		//img = cv::imread(WCHAR2String(fn));
		bool opened = vm->vidCap.open(WCHAR2String(fn));
		if (opened)
		{
			// ImgDoc构造内容
			//vidm1->vidCap >> img; //获取第一帧图像并显示
			vm->vidCap >> imgd->img;  // 获取第一帧图象
#ifdef _DEBUG
			DP0("Load First Image\n");
#endif // _DEBUG

			//coreData->vidNode1->setBMI();
			//CatchSize(hWnd);
			//coreData->vidNode1->imageConvert();
			if (imgd->img.empty() == false)
			{
				imgd->imageConvert();
				if (imgd->vidEffect == VideoEffect::edge)
				{
					cv::Mat edgeY, edgeX;
					cv::Sobel(imgd->img, edgeY, CV_8U, 1, 0);
					cv::Sobel(imgd->img, edgeX, CV_8U, 0, 1);
					imgd->img = edgeX + edgeY;
				}
				//TODO
				CatchSize(hWnd);
				//InvalidateRect(hWnd, NULL, false);
			}
			////激发WM_PAINT时间，让窗口重绘
			//InvalidateRect(hWnd, NULL, false);
		}
		else
		{
			MessageBox(
				hWnd,
				L"视频未能打开",
				L"错误提示",
				MB_OK
			);
		}
	}
}

void CatchCommandPlay(HWND hWnd)
{
	vidm1->vidState = PLAY_STATE::PLAYING;
	vidm2->vidState = PLAY_STATE::PLAYING;
}

void CatchCommandPause(HWND hWnd)
{
	vidm1->vidState = PLAY_STATE::PAUSED;
	vidm2->vidState = PLAY_STATE::PAUSED;
}

void CatchCommandStop(HWND hWnd)
{
	vidm1->vidState = PLAY_STATE::STOPPED;
	vidm1->vidCap.set(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES, 0);
	vidm2->vidState = PLAY_STATE::STOPPED;
	vidm2->vidCap.set(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES, 0);
}

void CatchCommandLine()
{
	ptType = PAINT_TYPE::TYPE_LINE;
#ifdef _DEBUG
	DP0("TYPE_LINE\n");
#endif // _DEBUG

}

void CatchCommandRect()
{
	ptType = PAINT_TYPE::TYPE_RECT;
}

void CatchCommandEllipse()
{
	ptType = PAINT_TYPE::TYPE_ELLIPSE;
}

void CatchCommandDrawClear()
{
	ptType = PAINT_TYPE::TYPE_NONE;
	paintData->clear();
}
