
// ZDStation4Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ZDStation4.h"
#include "ZDStation4Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace cv;

extern CRITICAL_SECTION g_CriticalSection;
const CvRect g_roi(1200, 600, 600, 600);
//红、绿、蓝、黄、紫、青
const Scalar g_colors[6] = {
	Scalar(0, 0, 255),
	Scalar(0, 255, 0),
	Scalar(255, 0, 0),
	Scalar(0, 255, 255),
	Scalar(255, 0, 255),
	Scalar(255, 255, 0)
};

UINT DectThread(LPVOID param)
{
	CZDStation4Dlg* pWnd = (CZDStation4Dlg*)param;
	Mat src, dst;
	while (pWnd->m_run_flag)
	{
		if (pWnd->m_open_camera_flag)
		{
			if (pWnd->m_mvs.isHaveImg(0))
			{
				pWnd->m_mvs.GetMatPic(src, 0);
				if (pWnd->m_caption_flag)
				{
					int rc = pWnd->DoCaption(src, dst);
					if (rc == 0)
					{
						pWnd->ShowResult(dst, true, false);
					}
					else
					{
						pWnd->ShowResult(src, false, false, rc);
					}
					pWnd->m_caption_flag = false;
				}
				else
				{
					int rc = pWnd->DoMainDect(src, dst);
					if (rc == 0)
					{
						pWnd->ShowResult(dst, true, true);
					}
					else
					{
						pWnd->ShowResult(src, false, true, rc);
					}
				}
			}
			else
				Sleep(10);
		}
		else
			Sleep(10);
	}
	return 0;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CZDStation4Dlg 对话框



CZDStation4Dlg::CZDStation4Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CZDStation4Dlg::IDD, pParent)
	, m_run_flag(true)
	, m_open_camera_flag(false)
	, m_caption_flag(false)
	, m_server_flag(false)
	, m_cnt(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_SAIMO);
	m_server = HP_Create_TcpServer(this);
}

void CZDStation4Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESULT_LIST, m_result_list);
}

BEGIN_MESSAGE_MAP(CZDStation4Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_CAPTIONS, &CZDStation4Dlg::OnBnClickedCaptions)
END_MESSAGE_MAP()


// CZDStation4Dlg 消息处理程序

BOOL CZDStation4Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	__init__();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CZDStation4Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CZDStation4Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CZDStation4Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



HBRUSH CZDStation4Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	CString str;
	switch (pWnd->GetDlgCtrlID())
	{
	case IDC_RESULT_STR:
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 255, 255));
		GetDlgItemText(IDC_RESULT_STR, str);
		if (str == _T("定位完成") || str == _T("标定完成"))
		{
			return m_greenBrush;
		}
		else if (str == _T("定位失败") || str == _T("标定失败") || str == _T("角度超出") 
			|| str == _T("中心失败") || str == _T("六点失败"))
		{
			return m_redBrush;
		}
		else
			return m_grayBrush;
		break;
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CZDStation4Dlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	for (int i = 1000; i <1010; i++)//因为是多个控件，所以这里用了循环
	{
		CWnd *pWnd = GetDlgItem(i);
		if (pWnd && nType != 1 && m_wndRect.Width() && m_wndRect.Height())  //判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
		{
			CRect rect;   //获取控件变化前的大小 
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);//将控件大小转换为在对话框中的区域坐标
			rect.left = rect.left*cx / m_wndRect.Width();//调整控件大小
			rect.right = rect.right*cx / m_wndRect.Width();
			rect.top = rect.top*cy / m_wndRect.Height();
			rect.bottom = rect.bottom*cy / m_wndRect.Height();
			pWnd->MoveWindow(rect);//设置控件大小 
		}
	}
	//改变尺寸时设置listCtrl的列宽
	if (m_result_list.m_hWnd)
	{
		CRect listRect;
		m_result_list.GetClientRect(&listRect);
		m_result_list.SetExtendedStyle(m_result_list.GetExtendedStyle() | LVS_EX_GRIDLINES
			| LVS_EX_FULLROWSELECT);
		m_result_list.SetColumnWidth(0, listRect.Width() / 5);
		m_result_list.SetColumnWidth(1, listRect.Width() / 4);
		m_result_list.SetColumnWidth(2, listRect.Width() / 4);
		m_result_list.SetColumnWidth(3, listRect.Width() - 2 * listRect.Width() / 4 - listRect.Width() / 5);
	}
	//重新获得窗口尺寸
	GetClientRect(&m_wndRect);
}


void CZDStation4Dlg::OnCancel()
{
	// TODO:  在此添加专用代码和/或调用基类
	if (m_run_flag)
		__exit__();
	CDialogEx::OnCancel();
}


void CZDStation4Dlg::OnOK()
{
	// TODO:  在此添加专用代码和/或调用基类
	if (m_run_flag)
		__exit__();
	CDialogEx::OnOK();
}


void CZDStation4Dlg::__init__()
{
	InitializeCriticalSection(&g_CriticalSection);
	GetClientRect(&m_wndRect);
	SetImgWindow(IDC_SHOW_IMG, _WINDOW_NAME_);
	ShowWindow(SW_MAXIMIZE);
	//设置颜色和字体
	m_bigFont.CreatePointFont(200, L"微软雅黑");
	GetDlgItem(IDC_RESULT_STR)->SetFont(&m_bigFont);
	m_greenBrush.CreateSolidBrush(RGB(0, 255, 0));
	m_redBrush.CreateSolidBrush(RGB(255, 0, 0));
	m_grayBrush.CreateSolidBrush(RGB(0x80, 0x80, 0x80));
	ReadCaptionPoints();
#ifdef _USE_GLOG_
	GLOG_init();
#endif
	CRect listRect;
	m_result_list.GetClientRect(&listRect);
	m_result_list.SetExtendedStyle(m_result_list.GetExtendedStyle() | LVS_EX_GRIDLINES
		| LVS_EX_FULLROWSELECT);
	m_result_list.InsertColumn(0, L"序号", LVCFMT_CENTER, listRect.Width() / 5);//使ctrlList只有一列
	m_result_list.InsertColumn(1, L"X偏差", LVCFMT_CENTER, listRect.Width() / 4);//使ctrlList只有一列
	m_result_list.InsertColumn(2, L"Z偏差", LVCFMT_CENTER, listRect.Width() / 4);//使ctrlList只有一列
	m_result_list.InsertColumn(3, L"偏转角度", LVCFMT_CENTER, 
		listRect.Width() - 2 * listRect.Width() / 4 - listRect.Width() / 5);//使ctrlList只有一列
	if (AfxBeginThread(DectThread, this) == nullptr)
		AfxMessageBox(L"创建线程失败！");
#ifndef _DONT_CONNECT_CAM_
	if (!OpenCamera())
		AfxMessageBox(L"打开相机失败！");
	else
		m_open_camera_flag = true;
	m_server_flag = m_server->Start(_TCP_SERVER_IP_, _TCP_SERVER_PORT_);
	if (!m_server_flag)
		AfxMessageBox(L"开启服务器失败！");
#endif
}

void CZDStation4Dlg::__exit__()
{
	m_run_flag = false;
	DeleteCriticalSection(&g_CriticalSection);
	CloseCamera();
#ifdef _USE_GLOG_
	if (m_log)
	{
		if (m_log->isOpend())
			m_log->GLOG_end_();
	}
#endif // _USE_GLOG_
	if (m_server_flag)
		m_server->Stop();
}

//初始化用，确定窗口控件和窗口名称
void CZDStation4Dlg::SetImgWindow(int Dlgitem, const char* str)
{
	cv::namedWindow(str, WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle(str);
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(Dlgitem)->m_hWnd);  //picture控件
	::ShowWindow(hParent, SW_HIDE);
}

//显示图像
void CZDStation4Dlg::ShowMatImg(cv::Mat src, int Dlgitem, const char* str)
{
	if (src.empty())
	{
		return;
	}
	cv::Mat bgr;
	if (src.channels() == 3)
		bgr = src.clone();
	else if (src.channels() == 1)
		cvtColor(src, bgr, CV_GRAY2BGR);
	else
		return;
	CRect m_rect;
	cv::Mat temp;
	GetDlgItem(Dlgitem)->GetClientRect(&m_rect);
	CvSize window_size = cvSize(m_rect.Width(), m_rect.Height());
	double width = window_size.width;
	double scale = (double)src.rows / (double)src.cols;
	double height = width*scale;
	if (width > window_size.width || height > window_size.height)
	{
		height = window_size.height;
		scale = (double)src.cols / (double)src.rows;
		width = height*scale;
		if (width > window_size.width || height > window_size.height)
		{
			return;
		}
	}
	cv::resize(bgr, temp, cvSize((int)width, (int)height));
	cv::Mat dst = cv::Mat::Mat(window_size, CV_8UC3, cvScalarAll(0xff));
	int x = (dst.cols - temp.cols) / 2;
	int y = (dst.rows - temp.rows) / 2;
	cv::Mat roi = dst(cvRect(x, y, temp.cols, temp.rows));
	temp.copyTo(roi);
	cv::imshow(str, dst);
}

//打开相机
bool CZDStation4Dlg::OpenCamera()
{
	std::vector<int> stationNum;
	stationNum.push_back(0);
	std::vector<MV_CAM_TRIGGER_MODE> mode;
	mode.push_back(MV_TRIGGER_MODE_ON);
	std::vector<MV_CAM_TRIGGER_SOURCE> source;
	source.push_back(MV_TRIGGER_SOURCE_SOFTWARE);
	std::vector<float> exposure;
	exposure.push_back(15000.0f);
	std::vector<float> gain;
	gain.push_back(0.0f);
	m_mvs.SetTriggerMode(mode);
	m_mvs.SetTriggerSource(source);
	m_mvs.SetExposureTime(exposure);
	m_mvs.SetGain(gain);
	if (!m_mvs.Open(stationNum))
		return false;
	return true;
}

void CZDStation4Dlg::CloseCamera()
{
	if (m_open_camera_flag)
	{
		std::vector<int> stationNum;
		stationNum.push_back(0);
		m_mvs.Close(stationNum);
		m_open_camera_flag = false;
	}
}

#ifdef _USE_GLOG_
void CZDStation4Dlg::GLOG_init()
{
	m_log = new GLOG();
	char info_file[35], error_file[35];
	SYSTEMTIME time;
	GetLocalTime(&time);
	CreateDirectory(L"D:/log", NULL);
	if (time.wMonth < 10)
	{
		sprintf_s(info_file, "D:/log/info_%4d_0%d.log", time.wYear, time.wMonth);
		sprintf_s(error_file, "D:/log/error_%4d_0%d.log", time.wYear, time.wMonth);
	}
	else
	{
		sprintf_s(info_file, "D:/log/info_%4d_%d.log", time.wYear, time.wMonth);
		sprintf_s(error_file, "D:/log/error_%4d_%d.log", time.wYear, time.wMonth);
	}
	m_log->GLOG_start_(info_file, error_file);
	m_log->INFO_LOG("-----六孔螺丝定位-----");
}
#endif

BOOL CZDStation4Dlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	//添加代码对话框背景贴图
	CRect   rect;
	GetClientRect(&rect);    //获取对话框长宽      
	CBitmap   bmpBackground;
	bmpBackground.LoadBitmap(IDB_BKIMG);    //载入资源中图片
	CDC   dcBmp;             //定义并创建一个内存设备环境
	dcBmp.CreateCompatibleDC(pDC);             //创建兼容性DC
	//将位图选入临时内存设备环境
	dcBmp.SelectObject(&bmpBackground);
	if (NULL == bmpBackground.GetSafeHandle())
	{
		return false;
	}
	BITMAP   m_bitmap;                         //图片变量               
	bmpBackground.GetBitmap(&m_bitmap);       //将图片载入位图中
	//调用函数显示图片StretchBlt显示形状可变
	pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &dcBmp, 0, 0, m_bitmap.bmWidth, m_bitmap.bmHeight, SRCCOPY);
	//return CDialogEx::OnEraseBkgnd(pDC);
	return TRUE;
}

//标定按钮
void CZDStation4Dlg::OnBnClickedCaptions()
{
	// TODO:  在此添加控件通知处理程序代码
	m_caption_flag = true;
	m_mvs.DoSoftwareOnce(0);
	/*Mat src = imread("D:/1122/200312/20200409170729772.bmp"), dst;
	int rc = DoMainDect(src, dst);
	if (rc == 0)
	{
		ShowResult(dst, true, true);
	}
	else
	{
		ShowResult(src, false, true, rc);
	}*/
}

int CZDStation4Dlg::WriteCaptionPoints()
{
	FileStorage fs("caption.xml", FileStorage::WRITE);
	if (!fs.isOpened())
	{
		return -1;
	}
	fs << "Center" << m_center;
	fs << "Point1" << m_luosipts[0];
	fs << "Point2" << m_luosipts[1];
	fs << "Point3" << m_luosipts[2];
	fs << "Point4" << m_luosipts[3];
	fs << "Point5" << m_luosipts[4];
	fs << "Point6" << m_luosipts[5];
	fs << "Point21" << m_luosipts2[0];
	fs << "Point22" << m_luosipts2[1];
	fs << "Point23" << m_luosipts2[2];
	m_single_flag = ((CButton*)GetDlgItem(IDC_CHECK_SINGLE))->GetCheck();
	fs << "Single_Flag" << m_single_flag;
	fs.release();
	return 0;
}

int CZDStation4Dlg::ReadCaptionPoints()
{
	FileStorage fs("caption.xml", FileStorage::READ);
	if (!fs.isOpened())
	{
		return -1;
	}
	fs["Center"] >> m_center;
	fs["Point1"] >> m_luosipts[0];
	fs["Point2"] >> m_luosipts[1];
	fs["Point3"] >> m_luosipts[2];
	fs["Point4"] >> m_luosipts[3];
	fs["Point5"] >> m_luosipts[4];
	fs["Point6"] >> m_luosipts[5];
	fs["Point21"] >> m_luosipts2[0];
	fs["Point22"] >> m_luosipts2[1];
	fs["Point23"] >> m_luosipts2[2];
	fs["Single_Flag"] >> m_single_flag;
	((CButton*)GetDlgItem(IDC_CHECK_SINGLE))->SetCheck(m_single_flag);
	fs.release();
	return 0;
}

int CZDStation4Dlg::getCenter(cv::Mat src, cv::Point3f &center)
{
	Mat gray;
	if (src.channels() == 1)
		gray = src.clone();
	else if (src.channels() == 3)
		cvtColor(src, gray, COLOR_BGR2GRAY);
	else return -2;
	cv::threshold(gray, gray, 50, 255, CV_THRESH_BINARY);
	std::vector<std::vector<cv::Point>> contours;
	findContours(gray.clone(), contours, RETR_LIST, CHAIN_APPROX_NONE);
	for (size_t x = 0; x < contours.size(); x++)
	{
		double tmpArea = contourArea(contours[x]);
		if (tmpArea > 800000)
		{
			RotatedRect rrect = cv::minAreaRect(contours[x]);
			if (fabs(rrect.size.width - rrect.size.height) > 20)
				continue;
			center.x = rrect.center.x; center.y = rrect.center.y;
			center.z = (rrect.size.height + rrect.size.width) / 4;
			return 0;
		}
	}
	return -1;
}

int CZDStation4Dlg::getSixPoint(cv::Mat src, std::vector<cv::Point3f> &points, cv::Point3f center, 
	int outr, int inrr)
{
	Mat gray;
	if (src.channels() == 1)
		gray = src.clone();
	else if (src.channels() == 3)
		cvtColor(src, gray, COLOR_BGR2GRAY);
	else return -2;
	points.clear();
	Mat mask = Mat::zeros(src.size(), CV_8UC1);
	circle(mask, Point(center.x, center.y), outr, cvScalarAll(0xff), -1);
	circle(mask, Point(center.x, center.y), inrr, cvScalarAll(0x00), -1);
	Mat temp; gray.copyTo(temp, mask);
	cv::threshold(temp, temp, _DECT_THRE_, 255, CV_THRESH_BINARY);
	std::vector<std::vector<cv::Point>> contours;
	findContours(temp.clone(), contours, RETR_LIST, CHAIN_APPROX_NONE);
	for (size_t x = 0; x<contours.size(); x++)
	{
		double tmpArea = contourArea(contours[x]);
		if (tmpArea > _SIX_CIRCLE_MIN_AREA_ && tmpArea < _SIX_CIRCLE_MAX_AREA_)
		{
			RotatedRect rrect = cv::minAreaRect(contours[x]);
			if (fabs(rrect.size.width - rrect.size.height) > _SUBWH_MIN_)
				continue;
			Point3f tmppt;
			tmppt.x = rrect.center.x; tmppt.y = rrect.center.y;
			tmppt.z = (rrect.size.height + rrect.size.width) / 4;
			points.push_back(tmppt);
		}
	}
	if (points.size() == 6)
		return 0;
	else
		return -1;
}

double CZDStation4Dlg::getLengthPoint(cv::Point3f pt1, cv::Point3f pt2)
{
	double dx = pt1.x - pt2.x;
	double dy = pt1.y - pt2.y;
	return sqrt(dx*dx + dy*dy);
}

double CZDStation4Dlg::getAngleOfLines(cv::Point3f pt1, cv::Point3f pt2, cv::Point3f pt3, cv::Point3f pt4)
{
	pt3 = pt1;
	if (pt2.x == pt1.x&&pt4.x == pt3.x)
	{
		return 0;
	}
	else if (pt2.x == pt1.x)
	{
		double dx = pt4.x - pt3.x;
		double dy = pt4.y - pt3.y;
		double angle = atan(dx / dy)* 180.0 / 3.1415926;
		return angle;
	}
	else if (pt4.x == pt3.x)
	{
		double dx = pt2.x - pt1.x;
		double dy = pt2.y - pt1.y;
		double angle = atan(dx / dy)* 180.0 / 3.1415926;
		return -angle;
	}
	else
	{
		double k1 = (pt2.y - pt1.y) / (pt2.x - pt1.x);
		double k2 = (pt4.y - pt3.y) / (pt4.x - pt3.x);
		float tan_k = 0; //直线夹角正切值
		float lines_arctan;//直线斜率的反正切值
		tan_k = (k2 - k1) / (1 + k2*k1); //求直线夹角的公式
		lines_arctan = atan(tan_k)* 180.0 / 3.1415926;
		return lines_arctan;
	}
}

double CZDStation4Dlg::getAngleOfLines(cv::Point3f pt1, cv::Point3f pt2, cv::Point3f pt3)
{
	double dx12 = pt2.x - pt1.x;
	double dx13 = pt3.x - pt1.x;
	double dy12 = pt2.y - pt1.y;
	double dy13 = pt3.y - pt1.y;
	double len12 = getLengthPoint(pt1, pt2);
	double len13 = getLengthPoint(pt1, pt3);
	double cosa = (dx12*dx13 + dy12*dy13) / (len12*len13);
	return acos(cosa)* 180.0 / 3.1415926;
}

int CZDStation4Dlg::rangePoints(cv::Point3f center, std::vector<cv::Point3f> pts, cv::Point3f* ptsdst)
{
	if (pts.size() != 6)
		return -1;
	int mind = 3072 + 2048;
	int idx = 0;
	for (int i = 0; i < 6; i++)
	{
		if (mind > pts[i].x + pts[i].y)
		{
			mind = pts[i].x + pts[i].y;
			idx = i;
		}
	}
	ptsdst[0] = pts[idx];
	pts.erase(pts.begin() + idx);
	for (size_t i = 0; i < pts.size(); i++)
	{
		double angle1 = 0, angle2 = 0;
		angle1 = getAngleOfLines(center, ptsdst[0], center, pts[i]);
		angle2 = getAngleOfLines(center, ptsdst[0], pts[i]);
		if (fabs(angle1 - 60) < 5 && fabs(angle2 - 60) < 5)
			ptsdst[1] = pts[i];
		else if (fabs(angle1 + 60) < 5 && fabs(angle2 - 120) < 5)
			ptsdst[2] = pts[i];
		else if (fabs(angle1) < 5 && fabs(angle2 - 180) < 5)
			ptsdst[3] = pts[i];
		else if (fabs(angle1 - 60) < 5 && fabs(angle2 - 120) < 5)
			ptsdst[4] = pts[i];
		else if (fabs(angle1 + 60) < 5 && fabs(angle2 - 60) < 5)
			ptsdst[5] = pts[i];
		else return -1;
	}
	return 0;
}

int CZDStation4Dlg::DoCaption(cv::Mat src, cv::Mat &dst)
{
	if (src.empty())
		return -1;
	Mat gray;
	if (src.channels() == 1)
	{
		gray = src.clone();
		cvtColor(src, dst, COLOR_GRAY2BGR);
	}
	else if (src.channels() == 3)
	{
		dst = src.clone();
		cvtColor(src, gray, COLOR_BGR2GRAY);
	}
	else return -2;
	if (getCenter(gray, m_center) != 0)
		return -1;
	rectangle(dst, g_roi, g_colors[4], 4);
	circle(dst, Point(m_center.x, m_center.y), m_center.z, g_colors[0], 4);
	circle(dst, Point(m_center.x, m_center.y), _LARGE_CIRCLE_R_, Scalar(255, 0, 0), 4);
	circle(dst, Point(m_center.x, m_center.y), _SMALL_CIRCLE_R_, Scalar(255, 0, 0), 4);
	std::vector<cv::Point3f> pts;
	if (getSixPoint(src, pts, m_center, _LARGE_CIRCLE_R_, _SMALL_CIRCLE_R_) != 0)
		return -1;
	for (size_t i = 0; i < pts.size(); i++)
		circle(dst, Point(pts[i].x, pts[i].y), pts[i].z, Scalar(0, 0, 255), 4);
	rangePoints(m_center, pts, m_luosipts);
	for (int i = 0; i < 6; i++)
	{
		if (i == 5)
			line(dst, Point(m_luosipts[0].x, m_luosipts[0].y), Point(m_luosipts[5].x, m_luosipts[5].y), g_colors[i], 4);
		else
			line(dst, Point(m_luosipts[i].x, m_luosipts[i].y), Point(m_luosipts[i + 1].x, m_luosipts[i + 1].y), g_colors[i], 4);
	}
	for (int i = 0; i < 3; i++)
	{
		if (m_single_flag)
			getCircleLinePoint(m_center, m_luosipts[2 * i], g_r, false, m_luosipts2[i]);
		else
			getCircleLinePoint(m_center, m_luosipts[2 * i + 1], g_r, false, m_luosipts2[i]);
	}
	for (int i = 0; i < 3; i++)
	{
		if (i == 2)
			line(dst, Point(m_luosipts2[0].x, m_luosipts2[0].y), Point(m_luosipts2[2].x, m_luosipts2[2].y), g_colors[i], 4);
		else
			line(dst, Point(m_luosipts2[i].x, m_luosipts2[i].y), Point(m_luosipts2[i + 1].x, m_luosipts2[i + 1].y), g_colors[i], 4);
	}
	WriteCaptionPoints();
	return 0;
}

int CZDStation4Dlg::DoMainDect(cv::Mat src, cv::Mat &dst)
{
	if (src.empty())
		return -1;
	Mat gray;
	if (src.channels() == 1)
	{
		gray = src.clone();
		cvtColor(src, dst, COLOR_GRAY2BGR);
	}
	else if (src.channels() == 3)
	{
		dst = src.clone();
		cvtColor(src, gray, COLOR_BGR2GRAY);
	}
	else return -2;
	if (getCenter(gray, m_ncenter) != 0)
		return -3;
	circle(dst, Point(m_ncenter.x, m_ncenter.y), m_ncenter.z, g_colors[0], 4);
	circle(dst, Point(m_ncenter.x, m_ncenter.y), _LARGE_CIRCLE_R_, Scalar(255, 0, 0), 4);
	circle(dst, Point(m_ncenter.x, m_ncenter.y), _SMALL_CIRCLE_R_, Scalar(255, 0, 0), 4);
	std::vector<cv::Point3f> pts;
	if (getSixPoint(src, pts, m_ncenter, _LARGE_CIRCLE_R_, _SMALL_CIRCLE_R_) != 0)
		return -4;
	for (size_t i = 0; i < pts.size(); i++)
		circle(dst, Point(pts[i].x, pts[i].y), pts[i].z, Scalar(0, 0, 255), 4);
	rangePoints(m_ncenter, pts, m_nluosipts);
	getAngleByLine(m_ncenter, m_luosipts[0], m_nluosipts);
	for (int i = 0; i < 6; i++)
	{
		if (i == 5)
			line(dst, Point(m_nluosipts[0].x, m_nluosipts[0].y), Point(m_nluosipts[5].x, m_nluosipts[5].y), g_colors[i], 4);
		else
			line(dst, Point(m_nluosipts[i].x, m_nluosipts[i].y), Point(m_nluosipts[i + 1].x, m_nluosipts[i + 1].y), g_colors[i], 4);
	}
	for (int i = 0; i < 3; i++)
	{
		if (m_single_flag)
			getCircleLinePoint(m_center, m_nluosipts[2 * i], g_r, false, m_nluosipts2[i]);
		else
			getCircleLinePoint(m_center, m_nluosipts[2 * i + 1], g_r, false, m_nluosipts2[i]);
	}
	//--------------
	if (m_nluosipts2[0].x > m_nluosipts2[1].x || m_nluosipts2[0].x>m_nluosipts2[2].x)
	{
		if (m_single_flag)
		{
			getCircleLinePoint(m_center, m_nluosipts[5], g_r, false, m_nluosipts2[0]);
			getCircleLinePoint(m_center, m_nluosipts[1], g_r, false, m_nluosipts2[1]);
			getCircleLinePoint(m_center, m_nluosipts[3], g_r, false, m_nluosipts2[2]);
		}
		else
		{
			getCircleLinePoint(m_center, m_nluosipts[0], g_r, false, m_nluosipts2[0]);
			getCircleLinePoint(m_center, m_nluosipts[2], g_r, false, m_nluosipts2[1]);
			getCircleLinePoint(m_center, m_nluosipts[3], g_r, false, m_nluosipts2[2]);
		}
	}
	//--------------
	for (int i = 0; i < 3; i++)
	{
		if (i == 2)
			line(dst, Point(m_nluosipts2[0].x, m_nluosipts2[0].y), Point(m_nluosipts2[2].x, m_nluosipts2[2].y), g_colors[i], 4);
		else
			line(dst, Point(m_nluosipts2[i].x, m_nluosipts2[i].y), Point(m_nluosipts2[i + 1].x, m_nluosipts2[i + 1].y), g_colors[i], 4);
	}
	float dx = m_center.x - m_ncenter.x;
	float dy = m_center.y - m_ncenter.y;
	m_nluosipts2[3].x = dy*_PIX_TO_MM_;
	m_nluosipts2[3].y = -dx*_PIX_TO_MM_;
#ifdef _USE_GLOG_
	char pl[50]; sprintf_s(pl, "中心偏移量：dx = %.2f dy = %.2f", dx, dy);
	if (m_log)
		m_log->INFO_LOG(pl);
#endif
	m_ncenter.x += dx;
	m_ncenter.y += dy;
	for (int i = 0; i < 6; i++)
	{
		m_nluosipts[i].x += dx;
		m_nluosipts[i].y += dy;
	}
	for (int i = 0; i < 3; i++)
	{
		m_nluosipts2[i].x += dx;
		m_nluosipts2[i].y += dy;
	}
	try
	{
		m_nangle = getAngleByLine(m_ncenter, m_luosipts[0], m_nluosipts, true);
	}
	catch (double ex)
	{
		if (ex == _OUT_OF_ANGLE_)
		{//角度超出
			return -5;
		}
	}
	
	for (int i = 0; i < 6; i++)
	{
		m_nluosipts[i].z = m_nangle;
	}
	for (int i = 0; i < 3; i++)
	{
		m_nluosipts2[i].z = m_nangle;
	}
	ReGoToPoint(dx, dy);
	return 0;
}

int CZDStation4Dlg::calcX1X2ciecle(double a, double b, double c, double &x1, double &x2)
{//ax^2+bx+c=0;
	double s = b*b - 4.0*a*c;
	if (s < 0)
		return -1;
	else if (s == 0)
	{
		x1 = (-1 * b + sqrt(s)) / (2 * a);
		x2 = (-1 * b + sqrt(s)) / (2 * a);
	}
	else
	{
		x1 = (-1 * b + sqrt(s)) / (2 * a);
		x2 = (-1 * b - sqrt(s)) / (2 * a);
	}
	return 0;
}

//计算过圆心的直线与圆的交点
int CZDStation4Dlg::getCircleLinePoint(cv::Point3f start, cv::Point3f end, double r,
	bool flag, cv::Point3f &result)
{
	Point3f result1, result2;
	result1.z = end.z;
	result2.z = end.z;
	if (start.x == end.x)
	{
		result1.x = end.x;
		result2.x = end.x;
		result1.y = end.y + r;
		result2.y = end.y - r;
	}
	else if (start.y == end.y)
	{
		result1.x = end.x + r;
		result2.x = end.x - r;
		result1.y = end.y;
		result2.y = end.y;
	}
	else
	{
		double k = (end.y - start.y) / (end.x - start.x);
		double _b = end.y - end.x*k;
		double m = end.x;
		double n = end.y;
		double A = k*k + 1;
		double B = 2 * (k*_b - k * n - m);
		double C = m*m + (_b - n)*(_b - n) - r*r;
		double x1, x2;
		if (calcX1X2ciecle(A, B, C, x1, x2) == 0)
		{
			result1.x = x1;
			result2.x = x2;
			result1.y = k*result1.x + _b;
			result2.y = k*result2.x + _b;
		}
		else
		{
			return -1;
		}
	}
	if (getLengthPoint(start, result2) > getLengthPoint(start, result1))
	{
		Point3f tmp = result2;
		result2 = result1;
		result1 = tmp;
	}
	if (flag)
		result = result1;
	else
		result = result2;
	return 0;
}

void CZDStation4Dlg::ShowResult(cv::Mat src, bool flag, bool send_flag, int mode)
{
	if (send_flag)
	{
		if (flag)
		{
			ShowResult(m_ncenter, m_nluosipts, m_nluosipts2);
			SendDataToRobot(true);
			SetDlgItemText(IDC_RESULT_STR, L"定位完成");
		}
		else
		{
			SendDataToRobot(false);
			switch (mode)
			{
			case 0:
				SetDlgItemText(IDC_RESULT_STR, L"定位完成");
				break;
			case -3:
				SetDlgItemText(IDC_RESULT_STR, L"中心失败");
				break;
			case -4:
				SetDlgItemText(IDC_RESULT_STR, L"六点失败");
				break;
			case -5:
				SetDlgItemText(IDC_RESULT_STR, L"角度超出");
				break;
			default:
				SetDlgItemText(IDC_RESULT_STR, L"定位失败");
				break;
			}
		}
	}
	else
	{
		if (flag)
		{
			ShowResult(m_center, m_luosipts, m_luosipts2);
			SetDlgItemText(IDC_RESULT_STR, L"标定完成");
		}
		else
		{
			switch (mode)
			{
			case 0:
				SetDlgItemText(IDC_RESULT_STR, L"标定完成");
				break;
			case -3:
				SetDlgItemText(IDC_RESULT_STR, L"中心失败");
				break;
			case -4:
				SetDlgItemText(IDC_RESULT_STR, L"六点失败");
				break;
			case -5:
				SetDlgItemText(IDC_RESULT_STR, L"角度超出");
				break;
			case -1:
			case -2:
			default:
				SetDlgItemText(IDC_RESULT_STR, L"标定失败");
				break;
			}
		}
	}
	ShowMatImg(src, IDC_SHOW_IMG, _WINDOW_NAME_);
}

void CZDStation4Dlg::ShowResult(cv::Point3f center, cv::Point3f* pts, cv::Point3f* pts2)
{
	ShowResult("C", center);
	ShowResult("R1", pts[0]);
	ShowResult("R2", pts[1]);
	ShowResult("R3", pts[2]);
	ShowResult("R4", pts[3]);
	ShowResult("R5", pts[4]);
	ShowResult("R6", pts[5]);
	ShowResult("L1", pts2[0]);
	ShowResult("L2", pts2[1]);
	ShowResult("L3", pts2[2]);
	m_cnt++;
}

void CZDStation4Dlg::ShowResult(const char* str, cv::Point3f pt)
{
	CString cstr;
	cstr.Format(L"%d_%s", m_cnt, (CString)str);
	int intm = m_result_list.InsertItem(0, cstr);
	cstr.Format(L"%.2f", pt.x);
	m_result_list.SetItemText(intm, 1, cstr);
	cstr.Format(L"%.2f", pt.y);
	m_result_list.SetItemText(intm, 2, cstr);
	cstr.Format(L"%.2f", pt.z);
	m_result_list.SetItemText(intm, 3, cstr);
}

double CZDStation4Dlg::getAngleByLine(cv::Point3f center, cv::Point3f ptstart, cv::Point3f* pts, bool calc_flag)
{
	double minangle1 = 360, minangle2 = 360;
	int idx1 = 0, idx2 = 0;
	for (int i = 0; i < 6; i++)
	{
		double angle1 = getAngleOfLines(center, ptstart, center, pts[i]);
		double angle2 = getAngleOfLines(center, ptstart, pts[i]);
		if (minangle2 > angle2&&angle1 > 0)
		{
			minangle2 = angle2;
			idx2 = i;
		}
		else if (minangle1 > angle2&&angle1 < 0)
		{
			minangle1 = angle2;
			idx1 = i;
		}
	}
	if (calc_flag)
	{
		minangle1 = -minangle1;
		int idx; double minalge;
		if (minangle1<_MIN_ROTATE_ANGLE_&&minangle2>_MAX_ROTATE_ANGLE_)
		{
#ifdef _USE_GLOG_
			if (m_log)
				m_log->ERROR_LOG("角度超出范围");
#endif
			throw _OUT_OF_ANGLE_;
			return -1;
		}
		else if (minangle1<_MIN_ROTATE_ANGLE_)
		{
			minalge = minangle2;
			idx = idx2;
		}
		else if (minangle2>_MAX_ROTATE_ANGLE_)
		{
			minalge = minangle1;
			idx = idx1;
		}
		else
		{
			if (fabs(minangle1) < fabs(minangle2))
			{
				minalge = minangle1;
				idx = idx1;
			}
			else
			{
				minalge = minangle2;
				idx = idx2;
			}
		}
		if (idx != 0)
		{
			cv::Point3f ptmp[6];
			for (int i = 0; i < 6; i++)
			{
				ptmp[i] = pts[idx % 6];
				idx++;
			}
			for (int i = 0; i < 6; i++)
			{
				pts[i] = ptmp[i];
			}
		}
		return minalge;
	}
	return 0;
}

void CZDStation4Dlg::ReGoToPoint(float dx, float dy)
{
	for (int i = 0; i < 6; i++)
	{
		m_nluosipts[i].x -= dx;
		m_nluosipts[i].y -= dy;
	}
	for (int i = 0; i < 3; i++)
	{
		m_nluosipts2[i].x -= dx;
		m_nluosipts2[i].y -= dy;
	}
	float x1, y1;
	for (int i = 0; i < 6; i++)
	{
		x1 = m_nluosipts[i].x;
		y1 = m_nluosipts[i].y;
		m_nluosipts[i].x = (m_luosipts[i].y - y1)*_PIX_TO_MM_;//-x
		m_nluosipts[i].y = -(m_luosipts[i].x - x1)*_PIX_TO_MM_;//z
	}
	for (int i = 0; i < 3; i++)
	{
		x1 = m_nluosipts2[i].x;
		y1 = m_nluosipts2[i].y;
		m_nluosipts2[i].x = (m_luosipts2[i].y - y1)*_PIX_TO_MM_;
		m_nluosipts2[i].y = -(m_luosipts2[i].x - x1)*_PIX_TO_MM_;
	}
	//--中心偏移
	for (int i = 0; i < 6; i++)
	{
		m_nluosipts[i].x += (dy)*_PIX_TO_MM_;
		m_nluosipts[i].y += -(dx)*_PIX_TO_MM_;
	}
	for (int i = 0; i < 3; i++)
	{
		m_nluosipts2[i].x += (dy)*_PIX_TO_MM_;
		m_nluosipts2[i].y += -(dx)*_PIX_TO_MM_;
	}
}

void CZDStation4Dlg::SendDataToRobot(bool flag)
{
	if (flag)
		m_server->Send(m_connect_id, (BYTE*)"1", 1);
	else
		m_server->Send(m_connect_id, (BYTE*)"0", 1);
}

void CZDStation4Dlg::SendDataToRobot(cv::Point3f pt)
{
	char pl[150];
	sprintf_s(pl, "%.2f;%.2f", pt.x, pt.y);
	std::string str(pl);
	m_server->Send(m_connect_id, (BYTE*)str.c_str(), str.length());
}

EnHandleResult CZDStation4Dlg::OnPrepareListen(ITcpServer * pSender, SOCKET soListen)
{//开始监听时调用
	return HR_OK;
}

EnHandleResult CZDStation4Dlg::OnAccept(ITcpServer * pSender, CONNID dwConnID, SOCKET soClient)
{//客户端连上时调用
	m_connect_id = dwConnID;
#ifdef _USE_GLOG_
	char str[50];
	sprintf_s(str, 50, "客户端\"%I64d\"连上", dwConnID);
	if (m_log)
		m_log->INFO_LOG(str);
#endif
	return HR_OK;
}

EnHandleResult CZDStation4Dlg::OnSend(ITcpServer * pSender, CONNID dwConnID, const BYTE * pData, int iLength)
{//发送消息时调用
	char *pl = new char[iLength + 1];
	memcpy_s(pl, iLength, (char*)pData, iLength);
	pl[iLength] = '\0';
#ifdef _USE_GLOG_
	char out[50];
	sprintf_s(out, 50, "发送\"%s\"", pl);
	if (m_log)
		m_log->INFO_LOG(out);
#endif
	if (pl)
		delete[] pl;
	return HR_OK;
}

EnHandleResult CZDStation4Dlg::OnReceive(ITcpServer * pSender, CONNID dwConnID, const BYTE * pData, int iLength)
{//接受到消息时调用
	char *pl = new char[iLength + 1];
	memcpy_s(pl, iLength, (char*)pData, iLength);
	pl[iLength] = '\0';
	std::string str(pl);
#ifdef _USE_GLOG_
	char out[50];
	sprintf_s(out, 50, "接收到\"%s\"", pl);
	if (m_log)
		m_log->INFO_LOG(out);
#endif
	if ((pl[0] == 's' || pl[0] == 'S') && (pl[1] == 't' || pl[1] == 'T'))
	{
		m_mvs.DoSoftwareOnce(0);
	}
	else if ((pl[0] == 'g' || pl[0] == 'G') && (pl[1] == 'e' || pl[1] == 'E') && (pl[2] == 't' || pl[2] == 'T'))
	{
		char pl[10]; sprintf_s(pl, "%.2f;", m_nangle);
		std::string str(pl);
		m_server->Send(m_connect_id, (BYTE*)str.c_str(), str.length());
	}
	else if ((pl[0] == 'r' || pl[0] == 'R') && (pl[1] == 'e' || pl[1] == 'E'))
	{
		if (pl[3] == '1')
			SendDataToRobot(m_nluosipts[0]);
		else if (pl[3] == '2')
			SendDataToRobot(m_nluosipts[1]);
		else if (pl[3] == '3')
			SendDataToRobot(m_nluosipts[2]);
		else if (pl[3] == '4')
			SendDataToRobot(m_nluosipts[3]);
		else if (pl[3] == '5')
			SendDataToRobot(m_nluosipts[4]);
		else if (pl[3] == '6')
			SendDataToRobot(m_nluosipts[5]);
#ifdef _FLP_GIVE_ZERO_
		else if (pl[3] == '7')
			SendDataToRobot(cv::Point3f(0, 0, 0));
		else if (pl[3] == '8')
			SendDataToRobot(cv::Point3f(0, 0, 0));
		else if (pl[3] == '9')
			SendDataToRobot(cv::Point3f(0, 0, 0));
#else
		else if (pl[3] == '7')
			SendDataToRobot(m_nluosipts2[0]);
		else if (pl[3] == '8')
			SendDataToRobot(m_nluosipts2[1]);
		else if (pl[3] == '9')
			SendDataToRobot(m_nluosipts2[2]);
#endif
		else if (pl[3] == '0')
			SendDataToRobot(m_nluosipts2[3]);
	}
	if (pl)
		delete[] pl;
	return HR_OK;
}

EnHandleResult CZDStation4Dlg::OnClose(ITcpServer * pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{//客户端断开时调用
	m_connect_id = -1;
#ifdef _USE_GLOG_
	char str[50];
	sprintf_s(str, 50, "客户端\"I64%d\"断开", dwConnID);
	if (m_log)
		m_log->INFO_LOG(str);
#endif
	return HR_OK;
}

EnHandleResult CZDStation4Dlg::OnShutdown(ITcpServer * pSender)
{//关闭时调用
	return HR_OK;
}

