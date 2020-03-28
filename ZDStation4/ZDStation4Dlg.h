
// ZDStation4Dlg.h : 头文件
//

#pragma once
#include "allDefine.h"
#include <opencv2\opencv.hpp>
#include <HPSocket.h>
#include "MVSOperator.h"

#define _TCP_SERVER_IP_			L"192.168.0.133"
#define _TCP_SERVER_PORT_		6000

#ifdef _USE_GLOG_
#include <GLOG.h>
#endif
#include "afxcmn.h"

// CZDStation4Dlg 对话框
class CZDStation4Dlg : public CDialogEx, public CTcpServerListener
{
// 构造
public:
	CZDStation4Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ZDSTATION4_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	void __init__();
	void __exit__();
	virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen);
	virtual EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, SOCKET soClient);
	virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult OnShutdown(ITcpServer* pSender);
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnCancel();
	virtual void OnOK();
	MVSOperator m_mvs;
	bool m_run_flag;
	bool m_open_camera_flag;
	CRect m_wndRect;
	void SetImgWindow(int Dlgitem, const char* str);
	void ShowMatImg(cv::Mat src, int Dlgitem, const char* str);
	bool OpenCamera();//打开相机
	void CloseCamera();
#ifdef _USE_GLOG_
	GLOG* m_log;
	void GLOG_init();
#endif
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedCaptions();
	int WriteCaptionPoints();
	int ReadCaptionPoints();
	int getCenter(cv::Mat src, cv::Point3f &center);
	int getSixPoint(cv::Mat src, std::vector<cv::Point3f> &points, cv::Point3f center, 
		int outr = _LARGE_CIRCLE_R_, int inrr = _SMALL_CIRCLE_R_);
	double getLengthPoint(cv::Point3f pt1, cv::Point3f pt2);
	double getAngleOfLines(cv::Point3f pt1, cv::Point3f pt2, cv::Point3f pt3, cv::Point3f pt4);
	double getAngleOfLines(cv::Point3f pt1, cv::Point3f pt2, cv::Point3f pt3);
	int rangePoints(cv::Point3f center, std::vector<cv::Point3f> pts, cv::Point3f* ptsdst);
	//标定的
	cv::Point3f m_center, m_luosipts[6], m_luosipts2[3];
	int DoCaption(cv::Mat src, cv::Mat &dst);
	//实测的
	cv::Point3f m_ncenter, m_nluosipts[6], m_nluosipts2[4];
	double m_nangle;
	int DoMainDect(cv::Mat src, cv::Mat &dst);
	bool m_caption_flag;
	int calcX1X2ciecle(double a, double b, double c, double &x1, double &x2);
	int getCircleLinePoint(cv::Point3f start, cv::Point3f end, double r,
		bool flag, cv::Point3f &result);
	void ShowResult(cv::Mat src, bool flag, bool send_flag);
	void ShowResult(cv::Point3f center, cv::Point3f* pts, cv::Point3f* pts2);
	void ShowResult(const char* str, cv::Point3f pt);
	double getAngleByLine(cv::Point3f center, cv::Point3f ptstart, cv::Point3f* pts, bool calc_flag = false);
	void ReGoToPoint(float dx, float dy);
	ITcpServer* m_server;
	CONNID m_connect_id;
	bool m_server_flag;
	CFont m_bigFont;
	CBrush m_greenBrush;
	CBrush m_redBrush;
	CBrush m_grayBrush;
	CListCtrl m_result_list;
	int m_cnt;
	void SendDataToRobot(bool flag);
	void SendDataToRobot(cv::Point3f pt);
	bool m_single_flag;
};
