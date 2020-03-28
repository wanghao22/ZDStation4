#pragma once

#include "MyCamera.h"
#include <fstream>
#include <vector>
#include <chrono>  
#include "allDefine.h"

typedef int Status;

class MVSOperator
{
public:
	MVSOperator();
	~MVSOperator();

	int m_nOnLineCameraNum; // on line camera quantities
	int m_nUsingCameraNum;  // indicate which camera when create work thread
	CMyCamera*   m_pcMyCamera[MAX_DEVICE_NUM];      // CMyCamera packed normal used interface
	void*  m_hGetOneFrameHandle[MAX_DEVICE_NUM]; // getoneframe thread handle
	BOOL m_bCaptureImgFinished[MAX_DEVICE_NUM];
	MV_CC_DEVICE_INFO_LIST m_stDevList;             // Device information list structure variable, to save device list
	int m_captureImgCnt;
	bool m_bCaptureImg[MAX_DEVICE_NUM];

	//定时报警
	bool m_bStartCaptureImg;
	int  m_nCaptureImgCostTime;
	bool m_bSystemState;

	/*Initialization*/
	::Status EnumDevice(void);
	::Status OpenDevice(std::vector<int>& index);                     // Open device

	/*Set parameter*/
	::Status DoSoftwareOnce(std::vector<int>& index);                    // Software trigger once
	::Status DoSoftwareOnce(int index);

	/*Basic function*/
	::Status StartGrabbing(std::vector<int>& index);                     // Start grabbing
	::Status StopGrabbing(std::vector<int>& index);                      // Stop grabbing

	/*Close and destroy*/
	::Status CloseDevice(std::vector<int>& index);                       // Close device
	::Status DestroyDevice(void);                     // Destroy device

	::Status ThreadFun(int nUsingDeviceNum);

	int GetCameraNumBySN(std::string cameraSN);

	
	bool GetMatPic(cv::Mat &img, int station);
	void Close(std::vector<int>& index);
	bool Open(std::vector<int>& index);

	void SetTriggerMode(std::vector<MV_CAM_TRIGGER_MODE>& mode){
		m_nTriggerMode = mode;
	}
	void SetExposureTime(std::vector<float> time){
		m_dExposureEdit = time;
	}
	void SetGain(std::vector<float> gain){
		m_dGainEdit = gain;
	}
	void SetTriggerSource(std::vector<MV_CAM_TRIGGER_SOURCE> source){
		m_nTriggerSource = source;
	}

	::Status SetTriggerMode(std::vector<int> index);                // 设置触发模式
	::Status SetExposureTime(std::vector<int> index);               // 设置曝光时间                     
	::Status SetGain(std::vector<int> index);                       // 设置增益
	::Status SetTriggerSource(std::vector<int> index);              // 设置触发源

	bool isHaveImg(int station);
	

	//相机操作相关变量和函数
private:
	BOOL  m_bRetStatus;                             // Function return state
	BOOL  m_bCreateDevice;                          // Create
	BOOL  m_bOpenDevice;                            // Open
	BOOL  m_bStartGrabbing;                         // Start grabbing
	std::vector<std::string> snArr;
	std::vector<cv::Mat> m_imgArr;
	bool m_have_img_flags[MAX_DEVICE_NUM];				//图像有没有被提取走

	std::vector<MV_CAM_TRIGGER_MODE>   m_nTriggerMode;                       // 触发模式
	std::vector<MV_CAM_TRIGGER_SOURCE>   m_nTriggerSource;                     // 触发源
	std::vector<float>  m_dExposureEdit;                    //曝光时间
	std::vector<float>  m_dGainEdit;                        // 增益   
};

