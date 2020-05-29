// VideoProcDemo_OpenCV.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "VideoProcDemo_OpenCV.h"
//#include "shobjidl_core.h
#include "ImageDoc.h"
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

ImageDoc* imgdoc1, * imgdoc2;
//cv::Mat img;


WCHAR FileNameOfVideo1[1024];                   // 视频1的文件路径和文件名
cv::VideoCapture VidCap1;                       // 视频1的读取器
enum PlayState
{
	playing, paused, stopped
};
PlayState playState = PlayState::stopped;       // 播放状态     
enum VideoEffect
{
	no, edge
};
VideoEffect vidEffect = VideoEffect::no;        // 视频画面效果

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
bool OpenVideoFile(HWND hWnd, LPWSTR* fn);
std::string WCHAR2String(LPCWSTR pwszSrc);


void CenterPaint(HWND hWnd, HDC dc);






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
	imgdoc1 = new ImageDoc();
	imgdoc2 = new ImageDoc();

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
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WCHAR* fn = (WCHAR*)FileNameOfVideo1;
	bool result;

	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_OPEN_VID1:
			result = OpenVideoFile(hWnd, &fn);
			if (result)
			{
				//img = cv::imread(WCHAR2String(fn));
				bool opened = VidCap1.open(WCHAR2String(fn));
				if (opened)
				{
					// ImgDoc构造内容
					//VidCap1 >> img; //获取第一帧图像并显示
					VidCap1 >> imgdoc1->img;  // 获取第一帧图象
					imgdoc1->setBMI();
					if (imgdoc1->img.empty() == false)
					{
						if (vidEffect == VideoEffect::edge)
						{
							cv::Mat edgeY, edgeX;
							cv::Sobel(imgdoc1->img, edgeY, CV_8U, 1, 0);
							cv::Sobel(imgdoc1->img, edgeX, CV_8U, 0, 1);
							imgdoc1->img = edgeX + edgeY;
						}

						InvalidateRect(hWnd, NULL, false);
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
			break;
		case IDM_PLAY_VID:
			playState = PlayState::playing;
			break;
		case IDM_PAUSE_VID:
			playState = PlayState::paused;
			break;
		case IDM_STOP_VID:
			playState = PlayState::stopped;
			VidCap1.set(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES, 0);
			break;
		case IDM_NO_EFFECT:
			vidEffect = VideoEffect::no;
			break;
		case IDM_EDGE_EFFECT:
			vidEffect = VideoEffect::edge;
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_TIMER:
		//DP0("WM_TIMER\n");
		if (VidCap1.isOpened() && playState == PlayState::playing)
		{
			VidCap1 >> imgdoc1->img;
			if (imgdoc1->img.empty() == false)
			{
				if (vidEffect == VideoEffect::edge)
				{
					cv::Mat edgeY, edgeX;
					cv::Sobel(imgdoc1->img, edgeY, CV_8U, 1, 0);
					cv::Sobel(imgdoc1->img, edgeX, CV_8U, 0, 1);
					imgdoc1->img = edgeX + edgeY;
				}

				InvalidateRect(hWnd, NULL, false);
			}
		}
		break;
	case WM_PAINT:
	{
		//DP0("WM_PAINT\n");
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		// 还未采用双缓冲
		CenterPaint(hWnd, hdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void CenterPaint(HWND hWnd, HDC dc) {
	if (imgdoc1->img.rows > 0)
	{
		switch (imgdoc1->img.channels())
		{
		case 1:
			cv::cvtColor(imgdoc1->img, imgdoc1->img, cv::COLOR_GRAY2BGR); // GRAY单通道
			break;
		case 3:
			cv::cvtColor(imgdoc1->img, imgdoc1->img, cv::COLOR_BGR2BGRA);  // BGR三通道
			break;
		default:
			break;
		}
		// 获取窗口的宽和高度
		//int nWindowWidth;
		LPRECT rect = new tagRECT;
		// 使用 GetClientRect 而非 GetWindowRect
		GetClientRect(hWnd, rect);
		int nWindowWidth = rect->right - rect->left;
		int nWindowHeight = rect->bottom - rect->top;
		//DP2("%d %d\n", nWindowWidth, nWindowHeight);
		double fWidthTimes = (double)nWindowWidth / imgdoc1->img.cols;
		double fHeightTimes = (double)nWindowHeight / imgdoc1->img.rows;

		DP2("bili %f %f\n", fWidthTimes, fHeightTimes);
		int nBeginWidth, nBeginHeight, nWidth, nHeight;
		if (fWidthTimes > fHeightTimes) {
			// Width加黑边
			nWidth = (int)(fHeightTimes * imgdoc1->img.cols);
			nBeginWidth = (nWindowWidth - nWidth) >> 1;
			nHeight = nWindowHeight;
			nBeginHeight = 0;
		}
		else {
			// Height加黑边
			nHeight = (int)(fWidthTimes * imgdoc1->img.rows);
			nBeginHeight = (nWindowHeight - nHeight) >> 1;
			nWidth = nWindowWidth;
			nBeginWidth = 0;
		}
		imgdoc1->resize(nWidth, nHeight);
		imgdoc1->changeBMI();
		//DP3("%d %d %d\n", nBeginHeight, nWidth,nHeight);
		//DP2("%d %d\n", imgdoc1->img.cols, imgdoc1->img.rows);

		//StretchDIBits(
		//	dc,
		//	0, 0, imgdoc1->img.cols, imgdoc1->img.rows,
		//	0, 0, imgdoc1->img.cols, imgdoc1->img.rows,
		//	imgdoc1->img.data,
		//	imgdoc1->bmi,
		//	DIB_RGB_COLORS,
		//	SRCCOPY
		//);
		StretchDIBits(
			dc,
			nBeginWidth, nBeginHeight, nWidth, nHeight,
			0, 0, imgdoc1->img.cols, imgdoc1->img.rows,
			//0, 0, nWidth, nHeight,
			imgdoc1->img.data,
			imgdoc1->bmi,
			DIB_RGB_COLORS,
			SRCCOPY
		);

	}
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
//————————————————
//版权声明：本文为CSDN博主「kingkee」的原创文章，遵循CC 4.0 BY - SA版权协议，转载请附上原文出处链接及本声明。
//原文链接：https ://blog.csdn.net/kingkee/java/article/details/98115024